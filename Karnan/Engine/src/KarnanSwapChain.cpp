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

    for (auto framebuffer : _swapChainFramebuffers) {
        vkDestroyFramebuffer(_karnanDevice.Device(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(_karnanDevice.Device(), _renderPass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(_karnanDevice.Device(), _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(_karnanDevice.Device(), _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(_karnanDevice.Device(), _inFlightFences[i], nullptr);
    }
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

void KarnanSwapChain::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
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

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_karnanDevice.Device(), &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void KarnanSwapChain::CreateFrameBuffers()
{
    _swapChainFramebuffers.resize(ImageCount());
    for (size_t i = 0; i < ImageCount(); i++) {
        std::array<VkImageView, 1> attachments = { _swapChainImageViews[i] };

        VkExtent2D swapChainExtent = GetSwapChainExtent();
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_karnanDevice.Device(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
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
