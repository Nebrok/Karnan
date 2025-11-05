#pragma once
#include "SimpleRenderSystem.h"

#include "Cube.h"
#include "KarnanCamera.h"
#include "KarnanFrameInfo.h"


class KarnanScene
{

public:

private:
	SimpleRenderSystem& _renderSystem;

	
	// Temp
	Cube* Triangle;
	GameObject* Viewer;
	KarnanCamera* Camera;

public:
	KarnanScene(SimpleRenderSystem& renderSystem);
	~KarnanScene();

	void LoadScene();
	void UpdateScene(float deltaTime);
	void RenderScene(Karnan::FrameInfo frameInfo);



private:



};