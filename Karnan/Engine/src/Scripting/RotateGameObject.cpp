#include "RotateGameObject.h"

#include "../GameObject.h"

void RotateGameObject::Init()
{
	_componentName = "RotateGameObject";
}

void RotateGameObject::Update(float deltaTime)
{
	_gameobject->Transform.Rotation.y += deltaTime * RotationSpeed;

}

void RotateGameObject::ImGuiRender()
{
	ImGui::DragFloat("Rotation Speed", &RotationSpeed, 0.0f);
}

CEREAL_REGISTER_DYNAMIC_INIT(RotateGameObject)