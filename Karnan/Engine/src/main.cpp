#include <iostream>


// Core Engine
#include "KarnanWindow.h"
#include "KarnanDevice.h"

const int WIDTH = 640;
const int HEIGHT = 480;


int main(int argc, char** argv)
{
	KarnanWindow karnanWindow{ WIDTH, HEIGHT };
	KarnanDevice karnanDevice{ karnanWindow };
	GLFWwindow* windowRef = karnanWindow.GetWindowReference();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(windowRef))
	{
		/* Swap front and back buffers */
		glfwSwapBuffers(windowRef);

		/* Poll for and process events */
		glfwPollEvents();
	}

	return 0;
}