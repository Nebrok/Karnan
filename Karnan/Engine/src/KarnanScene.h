#pragma once
#include "SimpleRenderSystem.h"

#include "Cube.h"
#include "KarnanCamera.h"
#include "KarnanFrameInfo.h"


class KarnanScene
{

public:
	// Temp
	Cube* Triangle;

private:
	SimpleRenderSystem& _renderSystem;

	std::vector<GameObject*> _gameObjects;

	// Temp
	GameObject* Plane;
	GameObject* Viewer;
	KarnanCamera* Camera;

public:
	KarnanScene(SimpleRenderSystem& renderSystem);
	~KarnanScene();

	void LoadScene();
	void UpdateScene(double deltaTime);
	void RenderScene(Karnan::FrameInfo frameInfo);

	bool RegisterGO(GameObject* gameObject);
	std::vector<GameObject*> GetAllGameObjects() { return _gameObjects; };


private:



};