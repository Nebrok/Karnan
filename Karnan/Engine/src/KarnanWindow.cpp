#include "KarnanWindow.h"

#include <stdexcept>



KarnanWindow::KarnanWindow(int width, int height)
	: _width(width), _height(height)
{
	/* Initialize the library */
	if (!glfwInit())
		throw std::runtime_error("Failed to Initialise GLFW!");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(_width, _height, "Hello World", NULL, NULL);
	if (!_window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to Initialise a GLFWwindow!");

	}

	/* Make the window's context current */
	glfwMakeContextCurrent(_window);

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
