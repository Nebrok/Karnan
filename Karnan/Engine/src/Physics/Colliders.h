#pragma once

#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "../GameObject.h"

class Collider
{
public:
	enum class ColliderType
	{
		NONE,
		BOX,
		SPHERE
	};

	ColliderType Type;
	GameObject* GameObject = nullptr;

	Collider() 
	{
		Type = ColliderType::NONE;
	};

	glm::mat4 Transform()
	{
		if (GameObject == nullptr)
			throw std::runtime_error("Collider gameobject pointer is null!");
		return GameObject->Transform.Mat4();
	}

	std::string ToString()
	{
		switch (Type)
		{
		case ColliderType::NONE:
			return "NONE";
		case ColliderType::BOX:
			return "BOX";
		case ColliderType::SPHERE:
			return "SPHERE";
		}
	}

private:


};

class SphereCollider : public Collider
{
public:
	float Radius = .5f;

	SphereCollider()
	{
		Type = ColliderType::SPHERE;
	}

	SphereCollider(float radius)
		:Radius(radius)
	{
		Type = ColliderType::SPHERE;
	}

};

class BoxCollider : public Collider
{
public:
	glm::vec3 Extent = { 0.5f, 0.5f, 0.5f };

	BoxCollider()
	{
		Type = ColliderType::BOX;
	}

	BoxCollider(const glm::vec3& extent)
		: Extent(extent)
	{
		Type = ColliderType::BOX;
	}

	std::vector<glm::vec3> GetAxises()
	{
		return {};
	}

};