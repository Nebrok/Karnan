#pragma once
#include "../SimpleRenderSystem.h"

#include "../KarnanCamera.h"
#include "../KarnanFrameInfo.h"

#include "../Boids/BoidManager.h"

class KarnanScene
{

public:

private:
	SimpleRenderSystem& _renderSystem;

	std::vector<std::shared_ptr<GameObject>> _gameObjects;

	// Temp
	GameObject* Plane;
	GameObject* Viewer;
	KarnanCamera* Camera;

	BoidManager* _boidManager;

public:
	KarnanScene(SimpleRenderSystem& renderSystem);
	~KarnanScene();

	void LoadScene();
	void UpdateScene(double deltaTime);
	void RenderScene(Karnan::FrameInfo frameInfo);

	bool RegisterGO(GameObject* gameObject);
	std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() { return _gameObjects; };

	void SerialiseScene();


private:



};