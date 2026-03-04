#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"


#include "KarnanWindow.h"
#include "KarnanDevice.h"
#include "KarnanRenderer.h"
#include "SceneManagement/KarnanScene.h"
#include "SceneManagement/SceneDataObject.h"


#include "Editor/KarnanEditor.h"

#include "InputManagementSystem.h"
//#include "SimpleRenderSystem.h"
#include "DeferredRenderSystem.h"
#include "MeshLoadingSystem.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/KarnanPhysics.h"
#include "PersistentStore.h"


const int WIDTH = 1480;
const int HEIGHT = 830;

class EngineCore
{

public:
	static EngineCore* Instance;



private:
	KarnanWindow _karnanWindow{WIDTH, HEIGHT};
	KarnanDevice _karnanDevice{ _karnanWindow };
	KarnanRenderer _karnanRenderer{ _karnanWindow, _karnanDevice };
	
	GLFWwindow* _windowRef;

	MeshLoadingSystem* _meshLoadingSystem;
	AssetManager* _assetManager;
	std::unique_ptr<InputManagementSystem> _inputManagementSystem;
	std::unique_ptr<DeferredRenderSystem> _renderSystem;
	std::unique_ptr<KarnanScene> _scene;
	std::unique_ptr<KarnanPhysics> _physicsEngine;
	std::shared_ptr<PersistentStore> _persistentStore;

	bool _editorMode = false;
	KarnanEditor* _editor;

	bool _playMode = true;

	std::string _startupScene = "EngineUsageRecording";

public:
	static EngineCore* StartupEngine();
	static void DestroyEngine();

	void Init();
	void Run();
	void LoadScene(std::string sceneName);
	void LoadScene(SceneDataObject& sceneData);

	bool PlayMode() { return _playMode; };
	void SetPlayMode(bool mode) { _playMode = mode; };

	void SetEditorMode(KarnanEditor* editor);

	std::shared_ptr<PersistentStore> GetPersistentStore() { return _persistentStore; };

	static KarnanDevice& Device() { return Instance->_karnanDevice; };
	static KarnanRenderer& Renderer() { return Instance->_karnanRenderer; };
	static GLFWwindow* Window() { return Instance->_windowRef; };
	static bool AddGameObjectToActiveScene(std::shared_ptr<GameObject> gameObject);
	static void DeleteGOFromActiveScene(uint32_t id);

	static std::vector<std::shared_ptr<GameObject>> GetAllGameObjectsInActiveScene();
	static std::vector<std::shared_ptr<GameObject>> GetAllGameObjectsInActiveSceneWithName(std::string);


	static std::string GetSceneName();
	static void SetSceneName(std::string sceneName);
	static KarnanScene* GetActiveScenePointer() { return Instance->_scene.get(); };
	static std::vector<CollisionEvent> GetCollisions() { return Instance->_physicsEngine->GetCollisionEvents(); };

private:
	EngineCore();
	~EngineCore();


};