#include "KarnanScene.h"

#include <iostream>

KarnanScene::KarnanScene(SimpleRenderSystem& renderSystem)
	: _renderSystem(renderSystem)
{
	

}

KarnanScene::~KarnanScene()
{
	delete(Triangle);
	delete(Viewer);
	delete(Camera);
}

void KarnanScene::LoadScene()
{
	Triangle = new Cube(_renderSystem.GetDevice());
	Viewer = new GameObject("CameraPosition");
	Camera = new KarnanCamera();

	Triangle->Transform.Scale = { 2.0f, 2.0f, 2.0f };

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
	//Triangle->Update(deltaTime);
	Camera->SetPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 25.f);
}

void KarnanScene::RenderScene(Karnan::FrameInfo frameInfo)
{
	Camera->SetPerspectiveProjection(glm::radians(50.f), frameInfo.Aspect, 0.1f, 25.f);
	_renderSystem.RenderObjects(frameInfo, *Camera, *Triangle);
}

bool KarnanScene::RegisterGO(GameObject* gameObject)
{
	_gameObjects.push_back(gameObject);
	return true;
}
