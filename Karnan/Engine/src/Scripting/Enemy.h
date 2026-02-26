#pragma once

#include "ScriptComponent.h"


class Enemy : public ScriptableComponent
{
public:


private:
	glm::vec3 _patrolExtent{ 1.0f, 0.0f, 1.0f };
	glm::vec3 _startingPoint{ 0.0f, 0.0f, 0.0f };

	glm::vec3 _patrolStart{ 0.0f, 0.0f, 0.0f };
	glm::vec3 _patrolEnd{ 0.0f, 0.0f, 0.0f };
	glm::vec3 _patrolStartToEnd{ 0.0f, 0.0f, 0.0f };

	float _patrolProgress = 0.5;
	float _patrolDir = 1.0f;
	float _patrolSpeed = 0.01f;

public:
	Enemy();
	~Enemy();

	void Init() override;
	void Start() override;
	void Update(float deltaTime) override;
	void ImGuiRender() override;

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<ScriptableComponent>(this));
		ar(cereal::make_nvp("PatrolExtent", _patrolExtent));
		ar(cereal::make_nvp("PatrolSpeed", _patrolSpeed));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<ScriptableComponent>(this));
		ar(cereal::make_nvp("PatrolExtent", _patrolExtent));
		ar(cereal::make_nvp("PatrolSpeed", _patrolSpeed));
	};

private:


};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(Enemy);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, Enemy);

CEREAL_FORCE_DYNAMIC_INIT(Enemy)