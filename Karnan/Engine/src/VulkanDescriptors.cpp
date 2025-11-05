#include "VulkanDescriptors.h"

// std
#include <cassert>
#include <stdexcept>

KarnanDescriptorSetLayout::Builder& KarnanDescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count) {
    assert(bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<KarnanDescriptorSetLayout> KarnanDescriptorSetLayout::Builder::build() const {
    return std::make_unique<KarnanDescriptorSetLayout>(_karnanDevice, bindings);
}

// *************** Descriptor Set Layout *********************

KarnanDescriptorSetLayout::KarnanDescriptorSetLayout(
    KarnanDevice& _karnanDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : _karnanDevice{ _karnanDevice }, bindings{ bindings } {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv : bindings) {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
        _karnanDevice.Device(),
        &descriptorSetLayoutInfo,
        nullptr,
        &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

KarnanDescriptorSetLayout::~KarnanDescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(_karnanDevice.Device(), descriptorSetLayout, nullptr);
}

// *************** Descriptor Pool Builder *********************

KarnanDescriptorPool::Builder& KarnanDescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType, uint32_t count) {
    poolSizes.push_back({ descriptorType, count });
    return *this;
}

KarnanDescriptorPool::Builder& KarnanDescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags) {
    poolFlags = flags;
    return *this;
}
KarnanDescriptorPool::Builder& KarnanDescriptorPool::Builder::setMaxSets(uint32_t count) {
    maxSets = count;
    return *this;
}

std::unique_ptr<KarnanDescriptorPool> KarnanDescriptorPool::Builder::build() const {
    return std::make_unique<KarnanDescriptorPool>(_karnanDevice, maxSets, poolFlags, poolSizes);
}

// *************** Descriptor Pool *********************

KarnanDescriptorPool::KarnanDescriptorPool(
    KarnanDevice& _karnanDevice,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize>& poolSizes)
    : _karnanDevice{ _karnanDevice } {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(_karnanDevice.Device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

KarnanDescriptorPool::~KarnanDescriptorPool() {
    vkDestroyDescriptorPool(_karnanDevice.Device(), descriptorPool, nullptr);
}

bool KarnanDescriptorPool::allocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(_karnanDevice.Device(), &allocInfo, &descriptor) != VK_SUCCESS) {
        return false;
    }
    return true;
}

void KarnanDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
    vkFreeDescriptorSets(
        _karnanDevice.Device(),
        descriptorPool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
}

void KarnanDescriptorPool::resetPool() {
    vkResetDescriptorPool(_karnanDevice.Device(), descriptorPool, 0);
}

// *************** Descriptor Writer *********************

KarnanDescriptorWriter::KarnanDescriptorWriter(KarnanDescriptorSetLayout& setLayout, KarnanDescriptorPool& pool)
    : setLayout{ setLayout }, pool{ pool } {}

KarnanDescriptorWriter& KarnanDescriptorWriter::writeBuffer(
    uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

KarnanDescriptorWriter& KarnanDescriptorWriter::writeImage(
    uint32_t binding, VkDescriptorImageInfo* imageInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

bool KarnanDescriptorWriter::build(VkDescriptorSet& set) {
    bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }
    overwrite(set);
    return true;
}

void KarnanDescriptorWriter::overwrite(VkDescriptorSet& set) {
    for (auto& write : writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool._karnanDevice.Device(), writes.size(), writes.data(), 0, nullptr);
}