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
	VkExtent2D _swapChainExtent;

	VkSwapchainKHR _swapChain;
	std::vector<VkImage> _swapChainImages;
	std::vector<VkImageView> _swapChainImageViews;

	std::vector<VkFramebuffer> _swapChainFramebuffers;
	VkRenderPass _renderPass;

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
	VkFramebuffer GetFrameBuffer(int index) { return _swapChainFramebuffers[index]; }
	VkRenderPass GetRenderPass() { return _renderPass; }

	float ExtentAspectRatio() {
		return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height);
	}

	bool CompareSwapFormats(const KarnanSwapChain& swapChain) const
	{
		return swapChain._swapChainImageFormat == _swapChainImageFormat;
	}

	VkResult AcquireNextImage(uint32_t* imageIndex);
	VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

private:
	void Init();

	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateFrameBuffers();
	void CreateSyncObjects();


	//Helpers
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};