#include "Door.h"

#include "../GameObject.h"
#include "../EngineCore.h"

#include "KeySystem.h"

Door::Door()
{
}

Door::~Door()
{
}

void Door::Init()
{
	ScriptableComponent::Init();
	_componentName = "Door";

}

void Door::Start()
{
	ScriptableComponent::Start();

}

void Door::Update(float deltaTime)
{
	ScriptableComponent::Update(deltaTime);

	std::vector<CollisionEvent> collisionEvents;
	for (auto collision : EngineCore::Instance->GetCollisions())
	{
		if (collision.GameobjectA.get() == _gameobject)
		{
			collisionEvents.push_back(collision);
		}
	}

	for (auto collision : collisionEvents)
	{
		if (collision.GameobjectB->HasTag("Player"))
		{
			GameObject* gameobject = collision.GameobjectB.get();
			KeySystem* keySystem = static_cast<KeySystem*>(gameobject->FindFirstComponentOfName("Key System"));
			if (keySystem == nullptr)
				break;
			if (keySystem->GetHasKey())
				OpenDoor();
			break;
		}
	}

}

void Door::ImGuiRender()
{
}

void Door::OpenDoor()
{
	_gameobject->Transform.Translation.y += 20.0f;
}

CEREAL_REGISTER_DYNAMIC_INIT(Door)
