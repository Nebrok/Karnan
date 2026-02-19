#pragma once
#include "../SimpleRenderSystem.h"

#include "../KarnanCamera.h"
#include "../Lights/PointLight.h"
#include "../KarnanFrameInfo.h"
#include "SceneDataObject.h"


class KarnanScene
{

public:

private:
	std::vector<std::shared_ptr<GameObject>> _gameObjects;

	KarnanCamera* Camera;

	std::string _sceneName = "NewScene";

public:
	KarnanScene();
	~KarnanScene();

	void LoadScene(std::string sceneName);
	void LoadScene(SceneDataObject& sceneData);
	void UpdateScene(double deltaTime);

	void CallStart();

	KarnanCamera* PrepareRenderInfo(float aspectRatio, std::vector<GameObject*>& lights, std::vector<GameObject*>& gameObjects);

	bool RegisterGO(std::shared_ptr<GameObject> gameObject);
	void DeleteGO(uint32_t goID);
	std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() { return _gameObjects; };

	void SerialiseScene();
	
	std::string GetName() { return _sceneName; };

private:



};