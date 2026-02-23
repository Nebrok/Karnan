#include "CameraFollowPlayer.h"

#include "../GameObject.h"
#include "../EngineCore.h"
#include "../KarnanCamera.h"

CameraFollow::CameraFollow()
{
}

CameraFollow::~CameraFollow()
{
}

void CameraFollow::Init()
{
	ScriptableComponent::Init();
	_componentName = "Camera Follow Player";

}

void CameraFollow::Start()
{
	ScriptableComponent::Start();

	std::vector<std::shared_ptr<GameObject>> gameObjects = EngineCore::Instance->GetAllGameObjectsInActiveScene();
	for (auto gameobject : gameObjects)
	{
		if (gameobject->HasTag("Main Camera"))
		{
			_camera = gameobject.get();
			std::cout << "The camera has been got" << '\n';
		}
	}



}

void CameraFollow::Update(float deltaTime)
{
	ScriptableComponent::Update(deltaTime);

	glm::vec3 backwardVector = _gameobject->Transform.ForwardVector() * -1.0f;

	float radius = 2.0f;

	glm::vec3 offset = { glm::cos(_gameobject->Transform.Rotation.z) * glm::cos(-_gameobject->Transform.Rotation.y) * radius,
						 glm::sin(_gameobject->Transform.Rotation.z) * radius,
						 glm::cos(_gameobject->Transform.Rotation.z) * glm::sin(-_gameobject->Transform.Rotation.y) * radius };


	glm::vec3 targetOffset = { 0.0f, 2.5f, 0.0f };
	glm::vec3 pivot = _gameobject->Transform.Translation + targetOffset;
	glm::vec3 forward = _gameobject->Transform.ForwardVector();

	//glm::vec3 cameraPoint = _gameobject->Transform.Translation + offset;
	glm::vec3 cameraPoint = pivot - forward * 5.0f;




	_camera->Transform.Translation = cameraPoint;
	KarnanCamera* camera = static_cast<KarnanCamera*>(_camera);

	camera->LookAt(pivot);
	//camera->SetViewTarget(_camera->Transform.Translation, _gameobject->Transform.Translation);

}

CEREAL_REGISTER_DYNAMIC_INIT(CameraFollow)
