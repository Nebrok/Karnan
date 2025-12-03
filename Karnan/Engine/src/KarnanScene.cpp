#include "KarnanScene.h"

#include <iostream>

KarnanScene::KarnanScene(SimpleRenderSystem& renderSystem)
	: _renderSystem(renderSystem)
{
	

}

KarnanScene::~KarnanScene()
{
	for (auto gameObject : _gameObjects)
	{
		delete gameObject;
	}
}

void KarnanScene::LoadScene()
{
	GameObject* Dragon = DBG_NEW GameObject("assets/Dragon_80K.obj");
	Dragon->CreateMesh("assets/Dragon_80K.obj");
	Dragon->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Dragon->Transform.Translation = { 0.0f, 2.0f, 0.0f };

	Camera = DBG_NEW KarnanCamera("MainCamera", { 5.0f, 3.0f, 5.0f });

	Plane = DBG_NEW GameObject("FishStaff");
	Plane->CreateMesh("assets/Fishstaff.obj");
	Plane->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Plane->Transform.Scale = { 1.0f, 1.0f, 1.0f };


	GameObject* Well = DBG_NEW GameObject("Well");
	Well->CreateMesh("assets/well.obj");
	Well->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Well->Transform.Translation = { -3.0f, 0.f, 0.f };

	GameObject* FoundationL5 = DBG_NEW GameObject("FoundationsLevel5");
	FoundationL5->CreateMesh("assets/FortificationsLevel5.obj");
	FoundationL5->CreateMaterial("assets/fortifications.png");
	FoundationL5->Transform.Translation = { 3.0f, 0.f, 0.f };

	_boidManager = DBG_NEW BoidManager();
}

void KarnanScene::UpdateScene(double deltaTime)
{
	_boidManager->UpdateBoids(deltaTime);
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
			renderableObjects.push_back(go);
	}

	_renderSystem.RenderObjects(frameInfo, *Camera, renderableObjects);
}

bool KarnanScene::RegisterGO(GameObject* gameObject)
{
	_gameObjects.push_back(gameObject);
	return true;
}
