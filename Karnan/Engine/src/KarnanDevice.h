#pragma once
#include "KarnanWindow.h"

#include "vulkan/vulkan.h"


//std libraries
#include <vector>



class KarnanDevice
{
public:
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

private:
	KarnanWindow* _window;


	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debugMessenger;




	// Validation Layers
	const std::vector<char const*> _validationLayers = { "VK_LAYER_KHRONOS_validation" };

public:
	KarnanDevice(KarnanWindow* window);
	~KarnanDevice();

private:
	void CreateInstance();
	void SetupDebugMessenger();

	bool CheckValidationLayerSupport();

	std::vector<const char*> GetRequiredExtensions();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};