#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "KarnanWindow.h"
#include "KarnanDevice.h"
#include "KarnanRenderer.h"
#include "KarnanScene.h"

#include "SimpleRenderSystem.h"
#include "BasicMesh.h"



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

	//ImGUI descriptor Pool
	VkDescriptorPool _guiDescriptorPool;

	std::unique_ptr<SimpleRenderSystem> _renderSystem;
	std::unique_ptr<KarnanScene> _scene;


public:
	static EngineCore* StartupEngine();
	static void DestroyEngine();


	void Init();
	void Run();
	void LoadScene();

	KarnanDevice& GetDevice() { return _karnanDevice; };

private:
	EngineCore();
	~EngineCore();


};