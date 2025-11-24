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
	Viewer = DBG_NEW GameObject("CameraPosition");
	Viewer->SetRenderable(false);
	Camera = DBG_NEW KarnanCamera();

	Plane = DBG_NEW GameObject("Viking House");
	Plane->CreateMesh("assets/Fishstaff.obj");
	Plane->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	Plane->Transform.Scale = { 1.0f, 1.0f, 1.0f };


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
