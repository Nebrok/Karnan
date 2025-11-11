#include "KarnanScene.h"


KarnanScene::KarnanScene(SimpleRenderSystem& renderSystem)
	: _renderSystem(renderSystem)
{
	Triangle = new Cube(_renderSystem.GetDevice());
	Viewer = new GameObject(_renderSystem.GetDevice());
	Camera = new KarnanCamera();

	Triangle->Transform.Scale = { 2.0f, 2.0f, 2.0f };

	Viewer->Transform.Translation = { -6.f, -2.f, -6.f };
	Camera->SetViewTarget(Viewer->Transform.Translation, Triangle->Transform.Translation);
	Camera->SetPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 25.f);

}

KarnanScene::~KarnanScene()
{
	delete(Triangle);
	delete(Viewer);
	delete(Camera);
}

void KarnanScene::LoadScene()
{

}

void KarnanScene::UpdateScene(float deltaTime)
{
	Triangle->Update(deltaTime);
	Camera->SetPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 25.f);
}

void KarnanScene::RenderScene(Karnan::FrameInfo frameInfo)
{
	Camera->SetPerspectiveProjection(glm::radians(50.f), frameInfo.Aspect, 0.1f, 25.f);
	_renderSystem.RenderObjects(frameInfo, *Camera, *Triangle);
}
