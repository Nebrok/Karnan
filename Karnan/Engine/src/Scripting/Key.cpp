#include "Key.h"

#include "../GameObject.h"
#include "../EngineCore.h"

#include "KeySystem.h"

Key::Key()
{
}

Key::~Key()
{
}

void Key::Init()
{
	ScriptableComponent::Init();
	
	_componentName = "Key";
}

void Key::Start()
{
	ScriptableComponent::Start();
}

void Key::Update(float deltaTime)
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
			keySystem->SetHasKey(true);
			EngineCore::Instance->DeleteGOFromActiveScene(_gameobject->GetId());
			break;
		}
	}

}

void Key::ImGuiRender()
{
	

}

CEREAL_REGISTER_DYNAMIC_INIT(Key)
