#include "KarnanScene.h"

#include <iostream>
#include <sstream>
#include <fstream>

//cereal serialisation
#include "cereal/archives/json.hpp"

#include "SceneDataObject.h"

KarnanScene::KarnanScene(SimpleRenderSystem& renderSystem)
	: _renderSystem(renderSystem)
{
	

}

KarnanScene::~KarnanScene()
{

}

void KarnanScene::LoadScene()
{
	SceneDataObject sceneData;
	sceneData.SceneName = "TestScene1";

	_gameObjects.clear();
	sceneData.LoadScene(_gameObjects);

	for (auto gameObject : _gameObjects)
	{
		if (gameObject->HasTag("Main Camera"))
		{
			Camera = dynamic_cast<KarnanCamera*>(gameObject.get());
		}
	}
	
	/*
	GameObject* Dragon = DBG_NEW GameObject("assets/Dragon_80K.obj");
	Dragon->CreateMesh("assets/Dragon_80K.obj");
	Dragon->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Dragon->Transform.Translation = { 1.5f, 2.0f, 0.0f };
	_gameObjects.push_back(std::shared_ptr<GameObject>(Dragon));

	Camera = DBG_NEW KarnanCamera("MainCamera");
	Camera->Transform.Translation = { 5.0f, 3.0f, 5.0f };
	_gameObjects.push_back(std::shared_ptr<GameObject>(Camera));

	Plane = DBG_NEW GameObject("FishStaff");
	Plane->CreateMesh("assets/Fishstaff.obj");
	Plane->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Plane->Transform.Translation = { -1.5f, 1.0f, 3.0f };
	Plane->Transform.Scale = { 1.0f, 1.0f, 1.0f };
	_gameObjects.push_back(std::shared_ptr<GameObject>(Plane));

	GameObject* Well = DBG_NEW GameObject("Well");
	Well->CreateMesh("assets/well.obj");
	Well->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Well->Transform.Translation = { -3.0f, -4.f, 0.f };
	_gameObjects.push_back(std::shared_ptr<GameObject>(Well));

	GameObject* FoundationL5 = DBG_NEW GameObject("FoundationsLevel5");
	FoundationL5->CreateMesh("assets/FortificationsLevel5.obj");
	FoundationL5->CreateMaterial("assets/fortifications.png");
	FoundationL5->Transform.Translation = { 7.0f, -4.f, 0.f };
	_gameObjects.push_back(std::shared_ptr<GameObject>(FoundationL5));


	GameObject* LionHead = DBG_NEW GameObject("Lion Head");
	LionHead->CreateMesh("assets/lion_head_4k.obj");
	LionHead->CreateMaterial("textures/lion_head_diff_4k.png");
	LionHead->Transform.Translation = { 0.0f, 0.f, 0.f };
	_gameObjects.push_back(std::shared_ptr<GameObject>(LionHead));
	*/

}

void KarnanScene::UpdateScene(double deltaTime)
{
	for (auto gameObject : _gameObjects)
	{
		gameObject->Update(deltaTime);
	}
}

void KarnanScene::RenderScene(Karnan::FrameInfo frameInfo)
{
	Camera->SetPerspectiveProjection(glm::radians(90.f), frameInfo.Aspect, 0.1f, 200.0f);
	_renderSystem.BindPipeline(frameInfo.commandBuffer);

	std::vector<GameObject*> renderableObjects;
	for(auto go : _gameObjects)
	{
		if (go->IsRenderable())
			renderableObjects.push_back(go.get());
	}

	_renderSystem.RenderObjects(frameInfo, *Camera, renderableObjects);
}

bool KarnanScene::RegisterGO(std::shared_ptr<GameObject> gameObject)
{
	_gameObjects.push_back(std::shared_ptr<GameObject>(gameObject));
	return true;
}

void KarnanScene::SerialiseScene()
{
	SceneDataObject sceneData;
	sceneData.SceneName = "TestScene1";
	sceneData.SaveScene(_gameObjects);

	//std::vector<std::shared_ptr<GameObject>> gameObjectsTemp;
	//sceneData.LoadScene(gameObjectsTemp);
}
