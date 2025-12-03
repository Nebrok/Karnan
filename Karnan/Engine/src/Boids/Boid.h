#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "../GameObject.h"

const int BOUNDING_BOX_HALFWIDTH = 20;

class Boid
{
private:
	glm::vec3 _velocity{ 0,0,0 };
	glm::vec3 _acceleration{ 0,0,0 };
	GameObject* _gameObject;

	float _maxSpeed = 6.f;

public:
	Boid();
	void Init();

	~Boid();

	void UpdateBoid(float deltaTime);

	glm::vec3 GetVelocity() { return _velocity; }
	void AddToVelocity(glm::vec3 velocity);
	void SetVelocity(glm::vec3 velocity);

	void AddToAcceleration(glm::vec3 acceleration);
	void SetAcceleration(glm::vec3 acceleration);

	GameObject* GetGameObject() { return _gameObject; }

private:


};