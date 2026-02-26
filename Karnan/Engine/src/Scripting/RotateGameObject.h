#pragma once
#include "ScriptComponent.h"

class RotateGameObject : public ScriptableComponent
{
public:
	float RotationSpeed = 0.1f;


public:
	RotateGameObject() {};
	~RotateGameObject() {};
	void Init() override;
	void Update(float deltaTime) override;
	void ImGuiRender() override;


	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<ScriptableComponent>(this));
		ar(CEREAL_NVP(RotationSpeed));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<ScriptableComponent>(this));
		ar(CEREAL_NVP(RotationSpeed));
	};

};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(RotateGameObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, RotateGameObject);

CEREAL_FORCE_DYNAMIC_INIT(RotateGameObject)