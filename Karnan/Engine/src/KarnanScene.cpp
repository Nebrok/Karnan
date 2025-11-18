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
	Triangle = DBG_NEW Cube("Cube");
	Viewer = DBG_NEW GameObject("CameraPosition");
	Viewer->SetRenderable(false);
	Camera = DBG_NEW KarnanCamera();

	Triangle->Transform.Scale = { 2.0f, 2.0f, 2.0f };


	Plane = DBG_NEW GameObject("Plane");
	Plane->CreateMesh("assets/Viking_House.obj");
	Plane->CreateMaterial("assets/Viking_House.png");

	Viewer->Transform.Translation = { 0.f, -2.f, -6.f };
	Camera->SetViewTarget(Viewer->Transform.Translation, Triangle->Transform.Translation);
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
