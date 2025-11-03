#include "EngineCore.h"

EngineCore::EngineCore()
{
	_windowRef = _karnanWindow.GetWindowReference();

	std::unique_ptr<SimpleRenderSystem> tempRenderer(new SimpleRenderSystem(_karnanDevice, _karnanRenderer.GetSwapChainRenderPass()));
	_renderSystem = move(tempRenderer);


}

EngineCore::~EngineCore()
{

}

void EngineCore::Init()
{
	std::unique_ptr<KarnanScene> newScene(new KarnanScene(*_renderSystem));
	_scene = move(newScene);

	_scene->LoadScene();
}

void EngineCore::Run()
{
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(_windowRef))
	{

		if (auto commandBuffer = _karnanRenderer.BeginFrame())
		{
			_renderSystem->BindPipeline(commandBuffer);
			_karnanRenderer.BeginSwapChainRenderPass(commandBuffer);

			_scene->UpdateScene(0.016f);
			_scene->RenderScene(commandBuffer);

			_karnanRenderer.EndSwapChainRenderPass(commandBuffer);
			_karnanRenderer.EndFrame();
		}

		/* Poll for and process events */
		glfwPollEvents();
	}

	vkDeviceWaitIdle(_karnanDevice.Device());
}
