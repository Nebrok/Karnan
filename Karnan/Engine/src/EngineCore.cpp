#include "EngineCore.h"

#include "KarnanFrameInfo.h"


#include <chrono>
#include <string>



EngineCore* EngineCore::Instance = nullptr;

bool EngineCore::AddGameObjectToActiveScene(GameObject* gameObject)
{
	return Instance->_scene->RegisterGO(gameObject);
}

std::vector<GameObject*> EngineCore::GetAllGameObjectsInActiveScene()
{
	return Instance->_scene->GetAllGameObjects();
}

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
	_meshLoadingSystem = std::unique_ptr<MeshLoadingSystem>(MeshLoadingSystem::StartMeshLoadingSystem());
}

void EngineCore::Run()
{
	std::unique_ptr<SimpleRenderSystem> tempRenderer(DBG_NEW SimpleRenderSystem(_karnanDevice, _karnanRenderer.GetSwapChainRenderPass(), KarnanSwapChain::MAX_FRAMES_IN_FLIGHT));
	_renderSystem = move(tempRenderer);

	LoadScene();

	int fpsFrameCount = 0;
	int fpsFramesRunning = 0;
	double frameUpdateRunningTime = 0;
	
	auto currentTime = std::chrono::high_resolution_clock::now();
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(_windowRef))
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		double frameTime = std::chrono::duration<double, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		int fps = 1 / frameTime;

		frameUpdateRunningTime += frameTime;
		fpsFrameCount++; 
		fpsFramesRunning += fps;
		if (frameUpdateRunningTime >= 0.2)
		{
			std::string fpsCount = "Hello Vulkan: " + std::to_string(fpsFramesRunning / fpsFrameCount);
			glfwSetWindowTitle(_windowRef, fpsCount.c_str());
			fpsFrameCount = 0;
			fpsFramesRunning = 0;
			frameUpdateRunningTime = 0;
		}
			
		_scene->UpdateScene(frameTime);

		if (_editorMode)
			_editor->Update();

		if (auto commandBuffer = _karnanRenderer.BeginFrame())
		{
			int frameIndex = _karnanRenderer.GetFrameIndex();
			Karnan::FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, _karnanRenderer.GetAspectRatio() };


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
