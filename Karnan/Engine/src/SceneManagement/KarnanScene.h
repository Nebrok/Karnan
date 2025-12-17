#pragma once
#include "../SimpleRenderSystem.h"

#include "../KarnanCamera.h"
#include "../Lights/PointLight.h"
#include "../KarnanFrameInfo.h"

#include "../Boids/BoidManager.h"

class KarnanScene
{

public:

private:
	SimpleRenderSystem& _renderSystem;

	std::vector<std::shared_ptr<GameObject>> _gameObjects;

	KarnanCamera* Camera;

	// Temp
	GameObject* Plane;
	GameObject* Viewer;

public:
	KarnanScene(SimpleRenderSystem& renderSystem);
	~KarnanScene();

	void LoadScene();
	void UpdateScene(double deltaTime);
	void RenderScene(Karnan::FrameInfo frameInfo);

	bool RegisterGO(std::shared_ptr<GameObject> gameObject);
	void DeleteGO(uint32_t goID);
	std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() { return _gameObjects; };

	void SerialiseScene();


private:



};