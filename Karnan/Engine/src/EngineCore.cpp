#include "EngineCore.h"

#include "KarnanGlobalUBO.h"
#include "KarnanFrameInfo.h"

#include "glm/gtc/type_ptr.hpp"

#include <chrono>



EngineCore* EngineCore::Instance = nullptr;

EngineCore::EngineCore()
{
	_windowRef = _karnanWindow.GetWindowReference();
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
	
}

void EngineCore::Run()
{
	std::unique_ptr<SimpleRenderSystem> tempRenderer(DBG_NEW SimpleRenderSystem(_karnanDevice, _karnanRenderer.GetSwapChainRenderPass(), KarnanSwapChain::MAX_FRAMES_IN_FLIGHT));
	_renderSystem = move(tempRenderer);

	LoadScene();

	auto currentTime = std::chrono::high_resolution_clock::now();
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(_windowRef))
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
			
		_scene->UpdateScene(frameTime);

		if (_editorMode)
			_editor->Update();

		if (auto commandBuffer = _karnanRenderer.BeginFrame())
		{
			int frameIndex = _karnanRenderer.GetFrameIndex();
			Karnan::FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, _karnanRenderer.GetAspectRatio() };

			_renderSystem->BindPipeline(commandBuffer);

			_karnanRenderer.BeginSwapChainRenderPass(commandBuffer);

			_scene->RenderScene(frameInfo);

			if (_editorMode)
				_editor->Render(commandBuffer);

			_karnanRenderer.EndSwapChainRenderPass(commandBuffer);
			_karnanRenderer.EndFrame();
		}

		/* Poll for and process events */
		glfwPollEvents();
	}

	vkDeviceWaitIdle(_karnanDevice.Device());

	
}

void EngineCore::LoadScene()
{
	std::unique_ptr<KarnanScene> newScene(DBG_NEW KarnanScene(*_renderSystem));
	_scene = move(newScene);

	_scene->LoadScene();
}

void EngineCore::SetEditorMode(KarnanEditor* editor)
{
	_editorMode = true;
	_editor = editor;
}
