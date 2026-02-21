#include "PlayerController.h"

#include <iostream>

#include "../GameObject.h"
#include "../InputManagementSystem.h"
#include "Physics.h"


void PlayerController::Init()
{
	ScriptableComponent::Init();

	_componentName = "PlayerController";

}

void PlayerController::Update(float deltaTime)
{
	ScriptableComponent::Update(deltaTime);

    glm::vec3 rotationDelta = { 0.0f, 0.0f, 0.0f };
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_left))
        rotationDelta.y--;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_right))
        rotationDelta.y++;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_up))
        rotationDelta.x--;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_down))
        rotationDelta.x++;

    if (glm::dot(rotationDelta, rotationDelta) > std::numeric_limits<float>::epsilon())
    {
        _gameobject->Transform.Rotation += _lookSpeed * (float)deltaTime * glm::normalize(rotationDelta);
    }

    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::space))
    {
        Physics* physicsComponent = static_cast<Physics*>(_gameobject->FindFirstComponentOfName("Physics"));
        if (physicsComponent == nullptr)
            return;
        physicsComponent->AddAcceleration({ 0.0f, 100.0f, 0.0f });
    }

}

CEREAL_REGISTER_DYNAMIC_INIT(PlayerController)