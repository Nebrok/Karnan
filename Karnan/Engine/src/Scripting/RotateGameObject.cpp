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
