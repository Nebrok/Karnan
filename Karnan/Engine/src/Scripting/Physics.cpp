#include "Physics.h"

#include <iostream>

#include "../GameObject.h"
#include "../EngineCore.h"


Physics::Physics()
{
}

Physics::~Physics()
{
}

void Physics::Init()
{
	ScriptableComponent::Init();
	_componentName = "Physics";

}

void Physics::Update(float deltaTime)
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

	//_gameobject->Transform.Translation += _velocity * deltaTime;
	//_velocity += _acceleration * deltaTime;
	//if (_velocity.length() <= _maxSpeed)
	//	_velocity = glm::normalize(_velocity) * _maxSpeed;

}

void Physics::ImGuiRender()
{

}

CEREAL_REGISTER_DYNAMIC_INIT(Physics)
