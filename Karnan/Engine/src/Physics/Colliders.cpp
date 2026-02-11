#include "Colliders.h"

#include "../GameObject.h"


Collider::Collider()
{
}

glm::mat4 Collider::Transform()
{
	if (GameObject == nullptr)
		throw std::runtime_error("Collider::Transform - Collider gameobject pointer is null!");
	return GameObject->Transform.Mat4();
}

glm::mat4 Collider::ScalelessTransform()
{
	if (GameObject == nullptr)
		throw std::runtime_error("Collider::ScalelessTransform - Collider gameobject pointer is null!");
	glm::mat4 transform = GameObject->Transform.Mat4();
	transform[0] /= GameObject->Transform.Scale.x;
	transform[1] /= GameObject->Transform.Scale.y;
	transform[2] /= GameObject->Transform.Scale.z;
	return transform;
}
