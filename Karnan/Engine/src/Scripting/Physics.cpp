#include "Physics.h"

#include <iostream>

#include "../GameObject.h"
#include "../EngineCore.h"
#include "../SpecialGameObjects/TerrainObject.h"


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

	_grounded = false;
	for (auto collision : collisionEvents)
	{
		if (collision.GameobjectB->HasTag("Terrain"))
		{
			TerrainObject* terrain = static_cast<TerrainObject*>(collision.GameobjectB.get());
			glm::vec2 gameobjectCoordinates = { _gameobject->Transform.Translation.x, _gameobject->Transform.Translation.z };
			if (!terrain->InTerrainBounds(gameobjectCoordinates))
				continue;
			float terrainHeight = terrain->HeightAt(gameobjectCoordinates);
			glm::vec3 ExtentInDirection = _gameobject->GetCollider()->ExtentInDirection({ 0.0f, -1.0f, 0.0f });
			glm::vec3 lowestPoint = _gameobject->Transform.Translation + ExtentInDirection;
			if (lowestPoint.y < terrainHeight)
			{
				_grounded = true;
				_gameobject->Transform.Translation.y += glm::abs(lowestPoint.y - terrainHeight);
			}
		}
		else
		{
			if (collision.HasCollisionPoint)
			{
				glm::vec3 centerToCollisionPoint = collision.Collision - _gameobject->Transform.Translation;
				float distanceToCollisionPoint = glm::length(centerToCollisionPoint);
				SphereCollider* sphereCollider = static_cast<SphereCollider*>(_gameobject->GetCollider().get());
				float collisionOverlap = glm::abs(distanceToCollisionPoint - sphereCollider->Radius);
				_gameobject->Transform.Translation += -glm::normalize(centerToCollisionPoint) * collisionOverlap;
			}
		}
		
		
	
	}


	_acceleration += _gravity;
	_velocity += _acceleration * deltaTime;
	_velocity *= 0.99f;

	if (_velocity.y >= _maxSpeed)
		_velocity.y = _maxSpeed;

	if (_grounded)
		_velocity.y = 0;

	_gameobject->Transform.Translation += _velocity * deltaTime;

	_acceleration = { 0.0f, 0.0f, 0.0f };
}

void Physics::AddVelocity(glm::vec3 additionalVelocity)
{
	_velocity += additionalVelocity;
}

void Physics::AddAcceleration(glm::vec3 additionalAcceleration)
{
	_acceleration += additionalAcceleration;
}

void Physics::ImGuiRender()
{

}

CEREAL_REGISTER_DYNAMIC_INIT(Physics)
