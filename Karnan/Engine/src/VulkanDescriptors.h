#pragma once

#include "KarnanDevice.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

class KarnanDescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(KarnanDevice& _karnanDevice) : _karnanDevice{ _karnanDevice } {}

        Builder& addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<KarnanDescriptorSetLayout> build() const;

    private:
        KarnanDevice& _karnanDevice;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    KarnanDescriptorSetLayout(
        KarnanDevice& _karnanDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~KarnanDescriptorSetLayout();
    KarnanDescriptorSetLayout(const KarnanDescriptorSetLayout&) = delete;
    KarnanDescriptorSetLayout& operator=(const KarnanDescriptorSetLayout&) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

private:
    KarnanDevice& _karnanDevice;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class KarnanDescriptorWriter;
};

class KarnanDescriptorPool {
public:
    class Builder {
    public:
        Builder(KarnanDevice& _karnanDevice) : _karnanDevice{ _karnanDevice } {}

        Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& setMaxSets(uint32_t count);
        std::unique_ptr<KarnanDescriptorPool> build() const;

    private:
        KarnanDevice& _karnanDevice;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    KarnanDescriptorPool(
        KarnanDevice& _karnanDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes);
    ~KarnanDescriptorPool();
    KarnanDescriptorPool(const KarnanDescriptorPool&) = delete;
    KarnanDescriptorPool& operator=(const KarnanDescriptorPool&) = delete;

    bool allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void resetPool();

private:
    KarnanDevice& _karnanDevice;
    VkDescriptorPool descriptorPool;

    friend class KarnanDescriptorWriter;
};

class KarnanDescriptorWriter {
public:
    KarnanDescriptorWriter(KarnanDescriptorSetLayout& setLayout, KarnanDescriptorPool& pool);

    KarnanDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    KarnanDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);

private:
    KarnanDescriptorSetLayout& setLayout;
    KarnanDescriptorPool& pool;
    std::vector<VkWriteDescriptorSet> writes;
};

