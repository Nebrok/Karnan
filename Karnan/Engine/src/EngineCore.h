#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "KarnanEditor.h"

#include "KarnanWindow.h"
#include "KarnanDevice.h"
#include "KarnanRenderer.h"
#include "KarnanScene.h"
#include "KarnanMainGUI.h"

#include "SimpleRenderSystem.h"
#include "MeshLoadingSystem.h"


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

	std::unique_ptr<MeshLoadingSystem> _meshLoadingSystem;
	std::unique_ptr<SimpleRenderSystem> _renderSystem;
	std::unique_ptr<KarnanScene> _scene;

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
	static bool AddGameObjectToActiveScene(GameObject* gameObject);
	static std::vector<GameObject*> GetAllGameObjectsInActiveScene();

private:
	EngineCore();
	~EngineCore();


};