#include "KarnanWindow.h"

#include <stdexcept>



KarnanWindow::KarnanWindow(int width, int height)
	: _width(width), _height(height)
{
	/* Initialize the library */
	if (!glfwInit())
		throw std::runtime_error("Failed to Initialise GLFW!");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(_width, _height, "Hello World", NULL, NULL);
	if (!_window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to Initialise a GLFWwindow!");

	}

	/* Make the window's context current */
	glfwMakeContextCurrent(_window);
	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, FrameBufferResizedCallback);
}

KarnanWindow::~KarnanWindow()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void KarnanWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Surface");
	}
}

void KarnanWindow::FrameBufferResizedCallback(GLFWwindow* window, int width, int height)
{
	auto karnanWindow = reinterpret_cast<KarnanWindow*>(glfwGetWindowUserPointer(window));
	karnanWindow->_frameBufferResized = true;
	karnanWindow->_width = width;
	karnanWindow->_height = height;
}
