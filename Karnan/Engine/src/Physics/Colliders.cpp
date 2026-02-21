#include "Colliders.h"

#include "../GameObject.h"


Collider::Collider()
{
}

glm::mat4 Collider::Transform()
{
	if (GameObject == nullptr)
		throw std::runtime_error("Collider::Transform - Collider gameobject pointer is null!");
	glm::mat4 transformation = GameObject->Transform.Mat4();
	transformation[3] += glm::vec4(ColliderOffset, 0.0f);
	return transformation;
}

glm::mat4 Collider::ScalelessTransform()
{
	if (GameObject == nullptr)
		throw std::runtime_error("Collider::ScalelessTransform - Collider gameobject pointer is null!");
	glm::mat4 transformation = GameObject->Transform.Mat4();
	transformation[3] += glm::vec4(ColliderOffset, 0.0f);
	transformation[0] /= GameObject->Transform.Scale.x;
	transformation[1] /= GameObject->Transform.Scale.y;
	transformation[2] /= GameObject->Transform.Scale.z;
	return transformation;
}

glm::vec3 Collider::Position()
{
	glm::vec3 translation = GameObject->Transform.Translation;
	translation += ColliderOffset;
	return translation;
}
