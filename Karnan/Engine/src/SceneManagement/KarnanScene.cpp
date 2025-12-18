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
	PointLight* firstLight = DBG_NEW PointLight();
	firstLight->Transform.Translation = { 1.0f, 1.0f, 1.0f };
	firstLight->SetColour({ 0.0f, 0.0f, 1.0f });
	_gameObjects.push_back(std::shared_ptr<PointLight>(firstLight));
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
	std::vector<GameObject*> sceneLights;
	for(auto go : _gameObjects)
	{
		if (go->IsRenderable())
			renderableObjects.push_back(go.get());

		if (PointLight* light = dynamic_cast<PointLight*>(go.get()); light != nullptr)
			sceneLights.push_back(go.get());
	}

	_renderSystem.RenderObjects(frameInfo, *Camera, sceneLights, renderableObjects);
}

bool KarnanScene::RegisterGO(std::shared_ptr<GameObject> gameObject)
{
	_gameObjects.push_back(std::shared_ptr<GameObject>(gameObject));
	return true;
}

void KarnanScene::DeleteGO(uint32_t goID)
{
	size_t indexToBeDeleted = -1;
	for (int i = 0; i < _gameObjects.size(); i++)
	{
		if (_gameObjects[i]->GetId() == goID)
		{
			indexToBeDeleted = i;
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

void KarnanScene::SerialiseScene()
{
	SceneDataObject sceneData;
	sceneData.SceneName = "TestScene1";
	sceneData.SaveScene(_gameObjects);
}
