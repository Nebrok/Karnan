#include <iostream>


// Core Engine
#include "KarnanWindow.h"
#include "KarnanDevice.h"
#include "KarnanRenderer.h"

#include "SimpleRenderSystem.h"
#include "BasicMesh.h"

const int WIDTH = 640;
const int HEIGHT = 480;


int main(int argc, char** argv)
{
	KarnanWindow karnanWindow{ WIDTH, HEIGHT };
	KarnanDevice karnanDevice{ karnanWindow };
	GLFWwindow* windowRef = karnanWindow.GetWindowReference();
	KarnanRenderer karnanRenderer{ karnanWindow, karnanDevice };

	BasicMesh triangle{ karnanDevice };

	SimpleRenderSystem simpleRenderSystem{ karnanDevice, karnanRenderer.GetSwapChainRenderPass() };

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(windowRef))
	{
		
		if (auto commandBuffer = karnanRenderer.BeginFrame())
		{
			simpleRenderSystem.BindPipeline(commandBuffer);
			karnanRenderer.BeginSwapChainRenderPass(commandBuffer);

			simpleRenderSystem.RenderObjects(commandBuffer, triangle);

			karnanRenderer.EndSwapChainRenderPass(commandBuffer);
			karnanRenderer.EndFrame();
		}

		/* Poll for and process events */
		glfwPollEvents();
	}

	vkDeviceWaitIdle(karnanDevice.Device());
	return 0;
}