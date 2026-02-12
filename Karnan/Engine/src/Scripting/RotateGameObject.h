#pragma once
#include "ScriptComponent.h"

class RotateGameObject : public ScriptableComponent
{
public:
	float RotationSpeed = 0.01f;


public:
	void Init() override;
	void Update(float deltaTime) override;

};