#include "KarnanScene.h"

#include <iostream>
#include <sstream>
#include <fstream>

//cereal serialisation
#include "cereal/archives/json.hpp"

#include "../KarnanCamera.h"
#include "../EngineCore.h"
#include "../Lights/PointLight.h"


KarnanScene::KarnanScene()
{
	

}

KarnanScene::~KarnanScene()
{

}

void KarnanScene::LoadScene(std::string sceneName)
{
	SceneDataObject sceneData;
	sceneData.SceneName = sceneName;
	_sceneName = sceneName;

	_gameObjects.clear();
	sceneData.LoadScene(_gameObjects);

	/*
	KarnanCamera* camera = DBG_NEW KarnanCamera("MainCamera");
	camera->Init();
	camera->AddMainCameraTag();
	_gameObjects.push_back(std::shared_ptr<KarnanCamera>(camera));
	*/


	for (auto gameObject : _gameObjects)
	{
		if (gameObject->HasTag("Main Camera"))
		{
			Camera = dynamic_cast<KarnanCamera*>(gameObject.get());
			break;
		}
	}
	BreakUpdateLoop();

	/*
	
	for (int i = 0; i < 50; i++)
	{
		PointLight* firstLight = DBG_NEW PointLight();
		firstLight->Transform.Translation = { float(rand() % 100 / 8.f), float(rand() % 100 / 8.f), float(rand() % 100 / 8.f) };
		firstLight->SetColour({ rand() % 100 / 100.f, rand() % 100 / 100.f, rand() % 100 / 100.f });
		firstLight->SetIntensity(.4f);
		firstLight->CreateMesh("./assets/models/icosphere.obj");
		firstLight->CreateMaterial("assets/materials/NULL_TEXTURE.kmat");
		firstLight->Transform.Scale = { 0.05f, 0.05f, 0.05f };
		_gameObjects.push_back(std::shared_ptr<PointLight>(firstLight));

	}
	
	for (int i = 0; i < 750; i++)
	{
		GameObject* dummy = DBG_NEW GameObject("Lion Head Asteroid");
		dummy->Transform.Translation = { float(rand() % 100 / 10.f), float(rand() % 100 / 10.f), float(rand() % 100 / 10.f) };
		dummy->Transform.Rotation = { float(rand() % 100 / 20.f), float(rand() % 100 / 20.f), float(rand() % 100 / 20.f) };
		dummy->CreateMesh("./assets/models/lion_head_4k.obj");
		dummy->CreateMaterial("assets/materials/LionHead.kmat");
		_gameObjects.push_back(std::shared_ptr<GameObject>(dummy));
	}
	
	GameObject* cube = DBG_NEW GameObject("Cube");
	cube->Transform.Translation = { 12.5f, -25.f, 12.5f };
	cube->Transform.Scale = { 25, 25, 25 };
	cube->CreateMesh("./assets/models/cube.obj");
	cube->CreateMaterial("assets/materials/Checker.kmat");
	_gameObjects.push_back(std::shared_ptr<GameObject>(cube));
	*/
	
	

}

void KarnanScene::LoadScene(SceneDataObject& sceneData)
{
	_gameObjects.clear();
	sceneData.LoadScene(_gameObjects);
	_sceneName = sceneData.SceneName;

	for (auto gameObject : _gameObjects)
	{
		if (gameObject->HasTag("Main Camera"))
		{
			Camera = dynamic_cast<KarnanCamera*>(gameObject.get());
			break;
		}
	}
	BreakUpdateLoop();
}

void KarnanScene::UpdateScene(double deltaTime)
{
	for (auto gameObject : _gameObjects)
	{
		if (gameObject->IsDead())
			continue;
		if (gameObject->HasTag("Main Camera"))
		{
			gameObject->Update(deltaTime);
			continue;
		}

		if (EngineCore::Instance->PlayMode())
			gameObject->Update(deltaTime);
		if (_breakUpdateLoop)
			break;
	}
	CleanScene();
	_breakUpdateLoop = false;
}

void KarnanScene::CallStart()
{
	for (auto gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

KarnanCamera* KarnanScene::PrepareRenderInfo(float aspectRatio, std::vector<GameObject*>& lights, std::vector<GameObject*>& gameObjects)
{
	Camera->SetProjection(aspectRatio);

	lights.clear();
	gameObjects.clear();

	for (auto go : _gameObjects)
	{
		if (go->IsRenderable())
			gameObjects.push_back(go.get());

		if (PointLight* light = dynamic_cast<PointLight*>(go.get()); light != nullptr)
			lights.push_back(go.get());
	}

	return Camera;
}

bool KarnanScene::RegisterGO(std::shared_ptr<GameObject> gameObject)
{
	_gameObjects.push_back(std::shared_ptr<GameObject>(gameObject));
	return true;
}

void KarnanScene::DeleteGO(uint32_t goID)
{
	_gameObjectsToBeDeleted.push_back(goID);
	for (int i = 0; i < _gameObjects.size(); i++)
	{
		if (_gameObjects[i]->GetId() == goID)
		{
			_gameObjects[i]->KillGameObject();
			break;
		}
	}
}

void KarnanScene::CleanScene()
{
	for (auto goID : _gameObjectsToBeDeleted)
	{
		size_t indexToBeDeleted = -1;
		for (int i = 0; i < _gameObjects.size(); i++)
		{
			if (_gameObjects[i]->GetId() == goID)
			{
				indexToBeDeleted = i;
				break;
			}
		}
		std::shared_ptr<GameObject> deletedGO;
		std::string objectName;
		uint32_t objectId;
		long numberRefs;
		if (indexToBeDeleted >= 0)
		{
			deletedGO = _gameObjects[indexToBeDeleted];
			objectName = deletedGO->ObjectName;
			objectId = deletedGO->GetId();
			numberRefs = deletedGO.use_count();
			_gameObjects.erase(_gameObjects.begin() + indexToBeDeleted);
		}
		std::cout << "GameObject deleted: " << objectName << " with ID: " << objectId << '\n';
		std::cout << "GameObject references at deletion: " << numberRefs << '\n';
	}
	_gameObjectsToBeDeleted.clear();
}

void KarnanScene::SerialiseScene()
{
	SceneDataObject sceneData;
	sceneData.SceneName = _sceneName;
	sceneData.SaveScene(_gameObjects);
}
