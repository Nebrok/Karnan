#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"


#include "KarnanWindow.h"
#include "KarnanDevice.h"
#include "KarnanRenderer.h"
#include "SceneManagement/KarnanScene.h"

#include "Editor/KarnanEditor.h"

#include "InputManagementSystem.h"
//#include "SimpleRenderSystem.h"
#include "DeferredRenderSystem.h"
#include "MeshLoadingSystem.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/KarnanPhysics.h"


const int WIDTH = 800;
const int HEIGHT = 800;

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

	bool _editorMode = false;
	KarnanEditor* _editor;

public:
	static EngineCore* StartupEngine();
	static void DestroyEngine();

	void Init();
	void Run();
	void LoadScene();

	void SetEditorMode(KarnanEditor* editor);

	static KarnanDevice& Device() { return Instance->_karnanDevice; };
	static KarnanRenderer& Renderer() { return Instance->_karnanRenderer; };
	static GLFWwindow* Window() { return Instance->_windowRef; };
	static bool AddGameObjectToActiveScene(std::shared_ptr<GameObject> gameObject);
	static void DeleteGOFromActiveScene(uint32_t id);
	static std::vector<std::shared_ptr<GameObject>> GetAllGameObjectsInActiveScene();

private:
	EngineCore();
	~EngineCore();


};