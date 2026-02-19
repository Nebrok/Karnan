#include "PlayerObject.h"

#include "../EngineCore.h"

PlayerObject::PlayerObject()
	: GameObject("Player")
{
}

PlayerObject::~PlayerObject()
{
}

void PlayerObject::Init()
{
	GameObject::Init();
}

void PlayerObject::Start()
{
	std::vector<std::shared_ptr<GameObject>> gameobjects = EngineCore::Instance->GetAllGameObjectsInActiveScene();
	for (auto go : gameobjects)
	{
		if (go->HasTag("Main Camera"))
		{
			_camera = static_cast<KarnanCamera*>(go.get());
		}
	}

	std::cout << "Player Start Called!" << '\n';
}

void PlayerObject::Update(double deltaTime)
{
	GameObject::Update(deltaTime);

}