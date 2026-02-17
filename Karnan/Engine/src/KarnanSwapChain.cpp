#include "KarnanSwapChain.h"

//std libs
#include <iostream>
#include <array>

KarnanSwapChain::KarnanSwapChain(KarnanDevice& deviceRef, VkExtent2D windowExtent)
    : _karnanDevice(deviceRef), _windowExtent(windowExtent)
{
    Init();
}

KarnanSwapChain::KarnanSwapChain(KarnanDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<KarnanSwapChain> previous)
    : _karnanDevice(deviceRef), _windowExtent(windowExtent), _oldKarnanSwapChain(previous)
{
    Init();

    //clean up old swap chain since it's no longer needed
    _oldKarnanSwapChain = nullptr;
}

KarnanSwapChain::~KarnanSwapChain()
{
    for (auto imageView : _swapChainImageViews) {
        vkDestroyImageView(_karnanDevice.Device(), imageView, nullptr);
    }
    _swapChainImageViews.clear();

    if (_swapChain != nullptr) {
        vkDestroySwapchainKHR(_karnanDevice.Device(), _swapChain, nullptr);
        _swapChain = nullptr;
    }

    for (int i = 0; i < _depthImages.size(); i++) {
        vkDestroyImageView(_karnanDevice.Device(), _depthImageViews[i], nullptr);
        vkDestroyImage(_karnanDevice.Device(), _depthImages[i], nullptr);
        vkFreeMemory(_karnanDevice.Device(), _depthImageMemorys[i], nullptr);
    }

    for (int i = 0; i < _positionImages.size(); i++) {
        vkDestroyImageView(_karnanDevice.Device(), _positionImageViews[i], nullptr);
        vkDestroyImage(_karnanDevice.Device(), _positionImages[i], nullptr);
        vkFreeMemory(_karnanDevice.Device(), _positionImageMemorys[i], nullptr);
    }

    for (int i = 0; i < _normalImages.size(); i++) {
        vkDestroyImageView(_karnanDevice.Device(), _normalImageViews[i], nullptr);
        vkDestroyImage(_karnanDevice.Device(), _normalImages[i], nullptr);
        vkFreeMemory(_karnanDevice.Device(), _normalImageMemorys[i], nullptr);
    }

    for (int i = 0; i < _albedoImages.size(); i++) {
        vkDestroyImageView(_karnanDevice.Device(), _albedoImageViews[i], nullptr);
        vkDestroyImage(_karnanDevice.Device(), _albedoImages[i], nullptr);
        vkFreeMemory(_karnanDevice.Device(), _albedoImageMemorys[i], nullptr);
    }

    for (auto framebuffer : _swapChainGeometryFramebuffers) {
        vkDestroyFramebuffer(_karnanDevice.Device(), framebuffer, nullptr);
    }

    for (auto framebuffer : _swapChainLightingFramebuffers) {
        vkDestroyFramebuffer(_karnanDevice.Device(), framebuffer, nullptr);
    }

    vkDestroySampler(_karnanDevice.Device(), _bufferSampler, nullptr);

    vkDestroyRenderPass(_karnanDevice.Device(), _geometryRenderPass, nullptr);
    vkDestroyRenderPass(_karnanDevice.Device(), _lightingRenderPass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(_karnanDevice.Device(), _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(_karnanDevice.Device(), _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(_karnanDevice.Device(), _inFlightFences[i], nullptr);
    }
}

VkFormat KarnanSwapChain::FindDepthFormat()
{
    return _karnanDevice.FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkResult KarnanSwapChain::AcquireNextImage(uint32_t* imageIndex)
{
    vkWaitForFences(_karnanDevice.Device(), 1, &_inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        _karnanDevice.Device(), 
        _swapChain, 
        std::numeric_limits<uint64_t>::max(), 
        _imageAvailableSemaphores[currentFrame], // must be a not signaled semaphore
        VK_NULL_HANDLE,
        imageIndex);

    return result;
}

VkResult KarnanSwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
{
    if (_imagesInFlight[*imageIndex] != VK_NULL_HANDLE) 
    {
        vkWaitForFences(_karnanDevice.Device(), 1, &_imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    _imagesInFlight[*imageIndex] = _inFlightFences[currentFrame];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(_karnanDevice.Device(), 1, &_inFlightFences[currentFrame]);
    if (vkQueueSubmit(_karnanDevice.GraphicsQueue(), 1, &submitInfo, _inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(_karnanDevice.PresentQueue(), &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

void KarnanSwapChain::Init()
{
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGBufferResources();
    CreateSampler();
    CreateGBufferDescriptorSets();
    CreateDepthResources();
    CreateFrameBuffers();
    CreateSyncObjects();
}

void KarnanSwapChain::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = _karnanDevice.GetSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _karnanDevice.Surface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = _karnanDevice.FindPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = _oldKarnanSwapChain == nullptr ? VK_NULL_HANDLE : _oldKarnanSwapChain->_swapChain;

    if (vkCreateSwapchainKHR(_karnanDevice.Device(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(_karnanDevice.Device(), _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_karnanDevice.Device(), _swapChain, &imageCount, _swapChainImages.data());

    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;
}

void KarnanSwapChain::CreateImageViews()
{
    _swapChainImageViews.resize(_swapChainImages.size());
    for (size_t i = 0; i < _swapChainImages.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _swapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = _swapChainImageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(_karnanDevice.Device(), &viewInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

    }

}

void KarnanSwapChain::CreateGBufferResources()
{
    VkExtent2D swapChainExtent = GetSwapChainExtent();
    
    // Positions Images and views
    {
        _positionImages.resize(ImageCount());
        _positionImageMemorys.resize(ImageCount());
        _positionImageViews.resize(ImageCount());

        for (int i = 0; i < _positionImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = _positionsFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            _karnanDevice.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _positionImages[i], _positionImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = _positionImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = _positionsFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_karnanDevice.Device(), &viewInfo, nullptr, &_positionImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }


    // Normal Images and views
    {
        _normalImages.resize(ImageCount());
        _normalImageMemorys.resize(ImageCount());
        _normalImageViews.resize(ImageCount());

        for (int i = 0; i < _normalImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = _normalFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            _karnanDevice.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _normalImages[i], _normalImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = _normalImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = _normalFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_karnanDevice.Device(), &viewInfo, nullptr, &_normalImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }


    // Albedo Images and views
    {
        _albedoImages.resize(ImageCount());
        _albedoImageMemorys.resize(ImageCount());
        _albedoImageViews.resize(ImageCount());

        for (int i = 0; i < _albedoImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = _albedoFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            _karnanDevice.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _albedoImages[i], _albedoImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = _albedoImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = _albedoFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_karnanDevice.Device(), &viewInfo, nullptr, &_albedoImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

}

void KarnanSwapChain::CreateSampler()
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    vkCreateSampler(_karnanDevice.Device(), &samplerInfo, nullptr, &_bufferSampler);
}

void KarnanSwapChain::CreateGBufferDescriptorSets()
{
    _gBufferDescriptorPool = KarnanDescriptorPool::Builder(_karnanDevice)
        .setMaxSets(100)
        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100)
        .build();


    auto lightingSetLayout = KarnanDescriptorSetLayout::Builder(_karnanDevice)
        .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .build();
    _gBufferDescriptorSetLayout = move(lightingSetLayout);

    _positionDescriptorImages.clear();
    for (int i = 0; i < ImageCount(); i++)
    {
        VkDescriptorImageInfo imageBufferInfo{};
        imageBufferInfo.sampler = _bufferSampler;
        imageBufferInfo.imageView = _positionImageViews[i];
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        _positionDescriptorImages.push_back(imageBufferInfo);
    }

    _normalDescriptorImages.clear();
    for (int i = 0; i < ImageCount(); i++)
    {
        VkDescriptorImageInfo imageBufferInfo{};
        imageBufferInfo.sampler = _bufferSampler;
        imageBufferInfo.imageView = _normalImageViews[i];
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        _normalDescriptorImages.push_back(imageBufferInfo);
    }

    _albedoDescriptorImages.clear();
    for (int i = 0; i < ImageCount(); i++)
    {
        VkDescriptorImageInfo imageBufferInfo{};
        imageBufferInfo.sampler = _bufferSampler;
        imageBufferInfo.imageView = _albedoImageViews[i];
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        _albedoDescriptorImages.push_back(imageBufferInfo);
    }

    _gBufferDescriptorSets.resize(ImageCount());
    KarnanDescriptorWriter writer = { *_gBufferDescriptorSetLayout, *_gBufferDescriptorPool };
    for (int i = 0; i < ImageCount(); i++)
    {
        writer.writeImage(0, &(_positionDescriptorImages[i]));
        writer.writeImage(1, &(_normalDescriptorImages[i]));
        writer.writeImage(2, &(_albedoDescriptorImages[i]));
        writer.build(_gBufferDescriptorSets[i]);
    }

}

void KarnanSwapChain::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat();
    _swapChainDepthFormat = depthFormat;
    VkExtent2D swapChainExtent = GetSwapChainExtent();

    _depthImages.resize(ImageCount());
    _depthImageMemorys.resize(ImageCount());
    _depthImageViews.resize(ImageCount());

    for (int i = 0; i < _depthImages.size(); i++) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        _karnanDevice.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,_depthImages[i],_depthImageMemorys[i]);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _depthImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(_karnanDevice.Device(), &viewInfo, nullptr, &_depthImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

void KarnanSwapChain::CreateRenderPass()
{
    VkAttachmentDescription positionsAttachment{}; 
    positionsAttachment.format = _positionsFormat;
    positionsAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    positionsAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    positionsAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    positionsAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    positionsAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    positionsAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    positionsAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference positionsAttachmentRef = {};
    positionsAttachmentRef.attachment = 0;
    positionsAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription normalsAttachment{};
    normalsAttachment.format = _normalFormat;
    normalsAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    normalsAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalsAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    normalsAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    normalsAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    normalsAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    normalsAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference normalsAttachmentRef = {};
    normalsAttachmentRef.attachment = 1;
    normalsAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription albedoAttachment{};
    albedoAttachment.format = _albedoFormat;
    albedoAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    albedoAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    albedoAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    albedoAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    albedoAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    albedoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference albedoAttachmentRef = {};
    albedoAttachmentRef.attachment = 2;
    albedoAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 3;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = GetSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription materialAttachment{};

    VkAttachmentReference geoColorReferences[] = { positionsAttachmentRef, normalsAttachmentRef, albedoAttachmentRef };
    VkSubpassDescription geometrySubpass = {};
    geometrySubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    geometrySubpass.colorAttachmentCount = 3;
    geometrySubpass.pColorAttachments = geoColorReferences;
    geometrySubpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependencyIn = {};
    dependencyIn.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencyIn.dstSubpass = 0;
    dependencyIn.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencyIn.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyIn.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencyIn.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencyIn.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkSubpassDependency dependencyOut = {};
    dependencyOut.srcSubpass = 0;
    dependencyOut.dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencyOut.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyOut.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencyOut.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencyOut.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencyOut.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    std::array<VkAttachmentDescription, 4> attachments = { positionsAttachment, normalsAttachment, albedoAttachment, depthAttachment };
    std::array<VkSubpassDependency, 2> dependencies = { dependencyIn, dependencyOut};
    VkRenderPassCreateInfo geoRenderPassInfo = {};
    geoRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    geoRenderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    geoRenderPassInfo.pAttachments = attachments.data();
    geoRenderPassInfo.subpassCount = 1;
    geoRenderPassInfo.pSubpasses = &geometrySubpass;
    geoRenderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    geoRenderPassInfo.pDependencies = dependencies.data();

    if (vkCreateRenderPass(_karnanDevice.Device(), &geoRenderPassInfo, nullptr, &_geometryRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create geometry render pass!");
    }

    //Lighting Render Pass

    VkSubpassDescription lightingSubpass = {};
    lightingSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    lightingSubpass.colorAttachmentCount = 1;
    lightingSubpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency lightingDependencyIn = {};
    lightingDependencyIn.srcSubpass = VK_SUBPASS_EXTERNAL;
    lightingDependencyIn.dstSubpass = 0;
    lightingDependencyIn.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    lightingDependencyIn.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    lightingDependencyIn.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    lightingDependencyIn.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    lightingDependencyIn.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo lightingRenderPassInfo = {};
    lightingRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    lightingRenderPassInfo.attachmentCount = 1;
    lightingRenderPassInfo.pAttachments = &colorAttachment;
    lightingRenderPassInfo.subpassCount = 1;
    lightingRenderPassInfo.pSubpasses = &lightingSubpass;
    lightingRenderPassInfo.dependencyCount = 1;
    lightingRenderPassInfo.pDependencies = &lightingDependencyIn;

    if (vkCreateRenderPass(_karnanDevice.Device(), &lightingRenderPassInfo, nullptr, &_lightingRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create lighting render pass!");
    }

}

void KarnanSwapChain::CreateFrameBuffers()
{
    VkExtent2D swapChainExtent = GetSwapChainExtent();
    _swapChainGeometryFramebuffers.resize(ImageCount());
    _swapChainLightingFramebuffers.resize(ImageCount());
    
    for (size_t i = 0; i < ImageCount(); i++) {
        std::array<VkImageView, 4> geometryAttachments = { _positionImageViews[i], _normalImageViews[i], _albedoImageViews[i], _depthImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _geometryRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(geometryAttachments.size());
        framebufferInfo.pAttachments = geometryAttachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_karnanDevice.Device(), &framebufferInfo, nullptr, &_swapChainGeometryFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create geometry framebuffer!");
        }
    }

    for (size_t i = 0; i < ImageCount(); i++) {
        std::array<VkImageView, 1> lightingAttachments = { _swapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _lightingRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(lightingAttachments.size());
        framebufferInfo.pAttachments = lightingAttachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_karnanDevice.Device(), &framebufferInfo, nullptr, &_swapChainLightingFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create lighting framebuffer!");
        }
    }


}

void KarnanSwapChain::CreateSyncObjects()
{
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    _imagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(_karnanDevice.Device(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_karnanDevice.Device(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_karnanDevice.Device(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

VkSurfaceFormatKHR KarnanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR KarnanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
  //for (const auto &availablePresentMode : availablePresentModes) {
  //  if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
  //    std::cout << "Present mode: Mailbox" << "\n";
  //    return availablePresentMode;
  //  }
  //}

  // for (const auto &availablePresentMode : availablePresentModes) {
  //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
  //     std::cout << "Present mode: Immediate" << "\n";
  //     return availablePresentMode;
  //   }
  // }

    std::cout << "Present mode: V-Sync" << "\n";
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D KarnanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D actualExtent = _windowExtent;
        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
