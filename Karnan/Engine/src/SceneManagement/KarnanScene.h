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
	std::vector<std::shared_ptr<GameObject>> _gameObjects;

	KarnanCamera* Camera;

	// Temp
	GameObject* Plane;
	GameObject* Viewer;

	std::string _sceneName = "PhysicsTestingScene";

public:
	KarnanScene();
	~KarnanScene();

	void LoadScene();
	void UpdateScene(double deltaTime);


	KarnanCamera* PrepareRenderInfo(float aspectRatio, std::vector<GameObject*>& lights, std::vector<GameObject*>& gameObjects);


	bool RegisterGO(std::shared_ptr<GameObject> gameObject);
	void DeleteGO(uint32_t goID);
	std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() { return _gameObjects; };

	void SerialiseScene();


private:



};