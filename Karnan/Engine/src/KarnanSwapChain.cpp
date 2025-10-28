#include "KarnanSwapChain.h"

//std libs
#include <iostream>

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
}

void KarnanSwapChain::Init()
{
    CreateSwapChain();
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
