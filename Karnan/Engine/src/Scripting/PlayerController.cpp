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

    if (glm::dot(rotationDelta, rotationDelta) > std::numeric_limits<float>::epsilon())
        _gameobject->Transform.Rotation += _lookSpeed * (float)deltaTime * glm::normalize(rotationDelta);


    glm::vec3 moveDir{ 0.0f, 0.0f, 0.0f };
    glm::vec3 forwardVector = _gameobject->Transform.ForwardVector();
    glm::vec3 leftVector = glm::cross(forwardVector, { 0.0f, 1.0f, 0.0f });


    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::w))
        moveDir += forwardVector;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::s))
        moveDir -= forwardVector;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::a))
        moveDir += leftVector;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::d))
        moveDir -= leftVector;

    if (moveDir.length() <= 0.0001f)
        moveDir = glm::normalize(moveDir);

    Physics* physicsComponent = static_cast<Physics*>(_gameobject->FindFirstComponentOfName("Physics"));
    if (physicsComponent == nullptr)
        return;

    physicsComponent->AddAcceleration(moveDir * 10.0f);

    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::space) && physicsComponent->IsGrounded() != false)
    {
        physicsComponent->AddAcceleration({ 0.0f, 1000.0f, 0.0f });
    }

}

CEREAL_REGISTER_DYNAMIC_INIT(PlayerController)