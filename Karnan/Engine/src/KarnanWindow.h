#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"



class KarnanWindow
{
private:
	GLFWwindow* _window;

	int _width;
	int _height;

public:
	KarnanWindow(int width, int height);
	~KarnanWindow();

	GLFWwindow* GetWindowReference() { return _window; };

	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:



};