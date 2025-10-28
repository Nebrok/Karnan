#pragma once
#include "KarnanDevice.h"

//std libs
#include <memory>


class KarnanSwapChain
{
public:

private:
	KarnanDevice& _karnanDevice;
	VkExtent2D _windowExtent;

	std::shared_ptr<KarnanSwapChain> _oldKarnanSwapChain;

	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;

	VkSwapchainKHR _swapChain;
	std::vector<VkImage> _swapChainImages;
	std::vector<VkImageView> _swapChainImageViews;


public:
	KarnanSwapChain(KarnanDevice& deviceRef, VkExtent2D windowExtent);
	KarnanSwapChain(KarnanDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<KarnanSwapChain> previous);
	~KarnanSwapChain();

	KarnanSwapChain(const KarnanSwapChain&) = delete;
	void operator=(const KarnanSwapChain&) = delete;

private:
	void Init();

	void CreateSwapChain();
	void CreateImageViews();


	//Helpers
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};