#include "EngineCore.h"

#include <chrono>

EngineCore* EngineCore::Instance = nullptr;

EngineCore::EngineCore()
{
	_windowRef = _karnanWindow.GetWindowReference();

	std::unique_ptr<SimpleRenderSystem> tempRenderer(DBG_NEW SimpleRenderSystem(_karnanDevice, _karnanRenderer.GetSwapChainRenderPass()));
	_renderSystem = move(tempRenderer);


}

EngineCore::~EngineCore()
{
}

EngineCore* EngineCore::StartupEngine()
{
	if (EngineCore::Instance == nullptr)
	{
		Instance = DBG_NEW EngineCore();
	}
	return Instance;
}

void EngineCore::DestroyEngine()
{
	delete(EngineCore::Instance);
}

void EngineCore::Init()
{
	std::unique_ptr<KarnanScene> newScene(DBG_NEW KarnanScene(*_renderSystem));
	_scene = move(newScene);

	_scene->LoadScene();
}

void EngineCore::Run()
{
	auto currentTime = std::chrono::high_resolution_clock::now();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(_windowRef))
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
			
		_scene->UpdateScene(frameTime);

		if (auto commandBuffer = _karnanRenderer.BeginFrame())
		{
			_renderSystem->BindPipeline(commandBuffer);
			_karnanRenderer.BeginSwapChainRenderPass(commandBuffer);

			_scene->RenderScene(commandBuffer);

			_karnanRenderer.EndSwapChainRenderPass(commandBuffer);
			_karnanRenderer.EndFrame();
		}

		/* Poll for and process events */
		glfwPollEvents();
	}

	vkDeviceWaitIdle(_karnanDevice.Device());
}
