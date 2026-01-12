#pragma once
#include "KarnanDevice.h"

//std libs
#include <memory>


class KarnanSwapChain
{
public:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

private:
	KarnanDevice& _karnanDevice;
	VkExtent2D _windowExtent;

	std::shared_ptr<KarnanSwapChain> _oldKarnanSwapChain;

	VkFormat _swapChainImageFormat;
	VkFormat _swapChainDepthFormat;
	VkExtent2D _swapChainExtent;

	VkSwapchainKHR _swapChain;
	
	std::vector<VkImage> _depthImages;
	std::vector<VkDeviceMemory> _depthImageMemorys;
	std::vector<VkImageView> _depthImageViews;
	std::vector<VkImage> _swapChainImages;
	std::vector<VkImageView> _swapChainImageViews;

	//GBuffer Resources
	VkFormat _positionsFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	std::vector<VkImage> _positionImages;
	std::vector<VkDeviceMemory> _positionImageMemorys;
	std::vector<VkImageView> _positionImageViews;

	VkFormat _normalFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	std::vector<VkImage> _normalImages;
	std::vector<VkDeviceMemory> _normalImageMemorys;
	std::vector<VkImageView> _normalImageViews;

	VkFormat _albedoFormat = VK_FORMAT_R8G8B8A8_UNORM;
	std::vector<VkImage> _albedoImages;
	std::vector<VkDeviceMemory> _albedoImageMemorys;
	std::vector<VkImageView> _albedoImageViews;


	std::vector<VkFramebuffer> _swapChainGeometryFramebuffers;
	std::vector<VkFramebuffer> _swapChainLightingFramebuffers;

	VkRenderPass _geometryRenderPass;
	VkRenderPass _lightingRenderPass;

	std::vector<VkSemaphore> _imageAvailableSemaphores;
	std::vector<VkSemaphore> _renderFinishedSemaphores;
	std::vector<VkFence> _inFlightFences;
	std::vector<VkFence> _imagesInFlight;
	size_t currentFrame = 0;


public:
	KarnanSwapChain(KarnanDevice& deviceRef, VkExtent2D windowExtent);
	KarnanSwapChain(KarnanDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<KarnanSwapChain> previous);
	~KarnanSwapChain();

	KarnanSwapChain(const KarnanSwapChain&) = delete;
	void operator=(const KarnanSwapChain&) = delete;

	VkFormat GetSwapChainImageFormat() { return _swapChainImageFormat; }
	size_t ImageCount() { return _swapChainImages.size(); }
	VkExtent2D GetSwapChainExtent() { return _swapChainExtent; }
	VkFramebuffer GetGeometryFrameBuffer(int index) { return _swapChainGeometryFramebuffers[index]; }
	VkFramebuffer GetLightingFrameBuffer(int index) { return _swapChainLightingFramebuffers[index]; }
	VkRenderPass GetGeometryRenderPass() { return _geometryRenderPass; }
	VkRenderPass GetLightingRenderPass() { return _lightingRenderPass; }

	VkFormat FindDepthFormat();


	float ExtentAspectRatio() {
		return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height);
	}

	bool CompareSwapFormats(const KarnanSwapChain& swapChain) const
	{
		return swapChain._swapChainDepthFormat == _swapChainDepthFormat && swapChain._swapChainImageFormat == _swapChainImageFormat;
	}

	VkResult AcquireNextImage(uint32_t* imageIndex);
	VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

private:
	void Init();

	void CreateSwapChain();
	void CreateImageViews();
	void CreateGBufferResources();
	void CreateDepthResources();
	void CreateRenderPass();
	void CreateFrameBuffers();
	void CreateSyncObjects();


	//Helpers
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};