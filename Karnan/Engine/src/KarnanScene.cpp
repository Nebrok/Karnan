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
	delete(Camera);
}

void KarnanScene::LoadScene()
{
	//GameObject* Cube = DBG_NEW GameObject("assets/cube.obj");
	//Cube->CreateMesh("assets/cube.obj");
	//Cube->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	//Cube->Transform.Translation = { 0.0f, 0.0f, 0.0f };

	GameObject* Dragon = DBG_NEW GameObject("assets/Dragon_80K.obj");
	Dragon->CreateMesh("assets/Dragon_80K.obj");
	Dragon->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Dragon->Transform.Translation = { 0.0f, 2.0f, 0.0f };

	Viewer = DBG_NEW GameObject("CameraPosition");
	Viewer->SetRenderable(false);
	Camera = DBG_NEW KarnanCamera();

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



	Viewer->Transform.Translation = { 0.f, 1.5f, -6.f };
	Camera->SetPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 25.f);
}

void KarnanScene::UpdateScene(double deltaTime)
{
	for (auto gameObject : _gameObjects)
	{
		gameObject->Update(deltaTime);
	}

	Camera->SetViewYXZ(Viewer->Transform.Translation, Viewer->Transform.Rotation);
	Camera->SetPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 25.f);
}

void KarnanScene::RenderScene(Karnan::FrameInfo frameInfo)
{
	Camera->SetPerspectiveProjection(glm::radians(50.f), frameInfo.Aspect, 0.1f, 25.f);
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
