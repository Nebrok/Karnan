#pragma once

#include "ScriptComponent.h"


class EndGoal : public ScriptableComponent
{
public:

private:

public:
	EndGoal();
	~EndGoal();

	void Init() override;
	void Start() override;
	void Update(float deltaTime) override;

private:


};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(EndGoal);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, EndGoal);

CEREAL_FORCE_DYNAMIC_INIT(EndGoal)