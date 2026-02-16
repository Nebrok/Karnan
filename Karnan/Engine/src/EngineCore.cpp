#include "EngineCore.h"

#include "KarnanFrameInfo.h"
#include "MessagingSystem/Messages.h"

#include <chrono>
#include <string>
#include <iostream>

#include "Scripting/ScriptComponent.h"


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

std::string EngineCore::GetSceneName()
{

	if (Instance->_scene != nullptr)
		return Instance->_scene->GetName();
	return "NewScene";
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
	ScriptRegister::RegisterTypes();


	_meshLoadingSystem = MeshLoadingSystem::StartMeshLoadingSystem();
	_assetManager = AssetManager::StartupAssetManager();
	_inputManagementSystem = std::unique_ptr<InputManagementSystem>(InputManagementSystem::StartupInputManagementSystem());
	_physicsEngine = std::unique_ptr<KarnanPhysics>(DBG_NEW KarnanPhysics());
	glfwSetScrollCallback(_windowRef, InputManagementSystem::InputSystemScrollCallback);
}

void EngineCore::Run()
{
	_renderSystem = std::unique_ptr<DeferredRenderSystem>(DBG_NEW DeferredRenderSystem(
		_karnanDevice, 
		_karnanRenderer.GetSwapChain(),
		KarnanSwapChain::MAX_FRAMES_IN_FLIGHT));


	_meshLoadingSystem->BeginProcessAsSeperateThread();

	std::shared_ptr<MLSGenerateBinaries> message = std::shared_ptr<MLSGenerateBinaries>(DBG_NEW MLSGenerateBinaries(Message::System::NONE));
	std::unique_lock<std::mutex> messageQueueLock(_meshLoadingSystem->MessageQueueMutex);
	_meshLoadingSystem->QueueMessage(message);
	messageQueueLock.unlock();
	
	_assetManager->LoadMaterialDataFromDisk();


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

		_assetManager->Process();

		if (_playMode)
		{
			_scene->UpdateScene(frameTime);

		}

		_physicsEngine->UpdatePhysics(_scene.get());
		//std::cout << "Number of collisions: " << _physicsEngine->GetCollisionEvents().size() << '\n';

		if (_editorMode)
		{
			_editor->Update();
		}

		if (auto commandBuffer = _karnanRenderer.BeginFrame())
		{
			std::vector<GameObject*> lights;
			std::vector<GameObject*> gameObjects;
			KarnanCamera* camera = _scene->PrepareRenderInfo(_karnanRenderer.GetAspectRatio(), lights, gameObjects);


			_karnanRenderer.BeginGeometryRenderPass(commandBuffer);

			_renderSystem->GeometryPass(commandBuffer, _karnanRenderer.GetFrameIndex(), camera, gameObjects);
			_karnanRenderer.EndGeometryRenderPass(commandBuffer);


			_karnanRenderer.ConfigureBarriers(commandBuffer);


			_karnanRenderer.BeginLightingRenderPass(commandBuffer);
			_renderSystem->LightingPass(commandBuffer, _karnanRenderer.GetFrameIndex(), _karnanRenderer.GetImageIndex(), camera, lights, _karnanRenderer.GetSwapChain());
			if (_editorMode)
				_editor->Render(commandBuffer);
			_karnanRenderer.EndLightingRenderPass(commandBuffer);

			_karnanRenderer.EndFrame();
		}

		/* Poll for and process events */
		_inputManagementSystem->UpdateKeyReads(_windowRef);
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
	std::unique_ptr<KarnanScene> newScene(DBG_NEW KarnanScene());
	_scene = move(newScene);

	_scene->LoadScene("PhysicsTestingScene");
}

void EngineCore::LoadScene(SceneDataObject& sceneData)
{
	if (_scene == nullptr)
	{
		std::unique_ptr<KarnanScene> newScene(DBG_NEW KarnanScene());
		_scene = move(newScene);
	}

	_scene->LoadScene(sceneData);
}

void EngineCore::SetEditorMode(KarnanEditor* editor)
{
	_editorMode = true;
	_editor = editor;
	_playMode = false;
}
