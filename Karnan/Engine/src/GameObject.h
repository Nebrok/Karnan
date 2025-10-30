#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Karnan
{
	struct Transform
	{
		glm::vec3 Position{ 0.f, 0.f, 0.f };
		glm::vec3 Rotation{ 0.f, 0.f, 0.f };
		glm::vec3 Scale{ 1.f, 1.f, 1.f };
	};
}



class GameObject 
{

public:

private:

public:
	GameObject();
	~GameObject();

private:


};