#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ScriptComponent.h"


class GameObject;

class CameraFollow : public ScriptableComponent
{
public:

private:
	GameObject* _camera = nullptr;
	glm::vec3 _cameraOffset = { 0.0f, 2.0f, -2.0f };


public:
	CameraFollow();
	~CameraFollow();

	void Init() override;
	void Start() override;
	void Update(float deltaTime) override;

private:


};


#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(CameraFollow);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, CameraFollow);

CEREAL_FORCE_DYNAMIC_INIT(CameraFollow)
