#include "EngineCore.h"

#include "KarnanFrameInfo.h"
#include "MessagingSystem/Messages.h"

#include <chrono>
#include <string>
#include <iostream>


EngineCore* EngineCore::Instance = nullptr;

bool EngineCore::AddGameObjectToActiveScene(std::shared_ptr<GameObject> gameObject)
{
	return Instance->_scene->RegisterGO(gameObject);
}

void EngineCore::DeleteGOFromActiveScene(uint32_t id)
{
	Instance->_scene->DeleteGO(id);
}

std::vector<std::shared_ptr<GameObject>> EngineCore::GetAllGameObjectsInActiveScene()
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
	_meshLoadingSystem = MeshLoadingSystem::StartMeshLoadingSystem();
	_assetManager = AssetManager::StartupAssetManager();
	_inputManagementSystem = std::unique_ptr<InputManagementSystem>(InputManagementSystem::StartupInputManagementSystem());
}

void EngineCore::Run()
{
	_renderSystem = std::unique_ptr<SimpleRenderSystem>(DBG_NEW SimpleRenderSystem(
		_karnanDevice, 
		_karnanRenderer.GetSwapChainRenderPass(), 
		KarnanSwapChain::MAX_FRAMES_IN_FLIGHT));


	_meshLoadingSystem->BeginProcessAsSeperateThread();

	std::shared_ptr<MLSGenerateBinaries> message = std::shared_ptr<MLSGenerateBinaries>(DBG_NEW MLSGenerateBinaries(Message::System::NONE));
	std::unique_lock<std::mutex> messageQueueLock(_meshLoadingSystem->MessageQueueMutex);
	_meshLoadingSystem->QueueMessage(message);
	messageQueueLock.unlock();
	
	LoadScene();

	int fpsFrameCount = 0;
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
		if (frameUpdateRunningTime >= 0.2)
		{
			std::string fpsCount = "Hello Vulkan: " + std::to_string(fpsFrameCount / frameUpdateRunningTime);
			glfwSetWindowTitle(_windowRef, fpsCount.c_str());
			fpsFrameCount = 0;
			frameUpdateRunningTime = 0;
		}

		_inputManagementSystem->UpdateKeyReads(_windowRef);
		_assetManager->Process();
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
	_scene->SerialiseScene();

	vkDeviceWaitIdle(_karnanDevice.Device());

	
	_meshLoadingSystem->EndProcessThread();
	MeshLoadingSystem::DestroyMeshLoadingSystem();
	AssetManager::DestroyAssetManager();
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
