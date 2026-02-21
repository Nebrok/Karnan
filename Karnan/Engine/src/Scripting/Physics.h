#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ScriptComponent.h"

class Physics : public ScriptableComponent
{

public:


private:
	glm::vec3 _velocity = { 0.0f, 0.0f, 0.0f };
	glm::vec3 _acceleration = { 0.0f, 0.0f, 0.0f };
	glm::vec3 _gravity = { 0.0f, -9.81f, 0.0f };
	float _maxSpeed = 12.0f;

public:
	Physics();
	~Physics();

	void Init() override;
	void Update(float deltaTime) override;

	void AddVelocity(glm::vec3 additionalVelocity);
	void AddAcceleration(glm::vec3 additionalAcceleration);


	void ImGuiRender();

private:

};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(Physics);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, Physics);

CEREAL_FORCE_DYNAMIC_INIT(Physics)
