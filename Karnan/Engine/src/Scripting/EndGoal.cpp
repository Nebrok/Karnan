#include "EndGoal.h"

#include "../GameObject.h"
#include "../EngineCore.h"

EndGoal::EndGoal()
{
}

EndGoal::~EndGoal()
{
}

void EndGoal::Init()
{
	ScriptableComponent::Init();
	_componentName = "End Goal";

}

void EndGoal::Start()
{
	ScriptableComponent::Start();
}

void EndGoal::Update(float deltaTime)
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
			EngineCore::Instance->LoadScene("Level2");
		}
	}

}


CEREAL_REGISTER_DYNAMIC_INIT(EndGoal)
