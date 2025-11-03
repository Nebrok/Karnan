#pragma once

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

private:
	KarnanWindow _karnanWindow{WIDTH, HEIGHT};
	KarnanDevice _karnanDevice{ _karnanWindow };
	KarnanRenderer _karnanRenderer{ _karnanWindow, _karnanDevice };

	GLFWwindow* _windowRef;

	std::unique_ptr<SimpleRenderSystem> _renderSystem;
	std::unique_ptr<KarnanScene> _scene;


public:
	EngineCore();
	~EngineCore();

	void Init();

	void Run();



private:


};