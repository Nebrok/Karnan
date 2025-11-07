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
	bool _frameBufferResized = false;

public:
	KarnanWindow(int width, int height);
	~KarnanWindow();

	//Deletes the copy constructor and operator
	KarnanWindow(const KarnanWindow&) = delete;
	KarnanWindow& operator=(const KarnanWindow&) = delete;

	bool ShouldClose() { return glfwWindowShouldClose(_window); };

	GLFWwindow* GetWindowReference() { return _window; };
	VkExtent2D GetExtent() { return { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height) }; }

	bool WasWindowResized() { return _frameBufferResized; }
	void ResetWindowFlag() { _frameBufferResized = false; }

	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
	static void FrameBufferResizedCallback(GLFWwindow* window, int width, int height);



};