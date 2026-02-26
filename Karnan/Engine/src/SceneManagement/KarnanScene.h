#pragma once
#include "SceneDataObject.h"

class KarnanCamera;

class KarnanScene
{

public:

private:
	std::vector<std::shared_ptr<GameObject>> _gameObjects;
	std::vector<uint32_t> _gameObjectsToBeDeleted;

	KarnanCamera* Camera;


	std::string _sceneName = "NewScene";

	bool _breakUpdateLoop = false;

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
	void CleanScene();
	std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() { return _gameObjects; };

	void SerialiseScene();
	
	std::string GetName() { return _sceneName; };
	void SetName(std::string sceneName) { _sceneName = sceneName; };

	void BreakUpdateLoop() { _breakUpdateLoop = true; };

private:



};