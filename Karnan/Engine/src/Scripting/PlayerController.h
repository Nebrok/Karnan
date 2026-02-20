#pragma once
#include "ScriptComponent.h"

class PlayerController : public ScriptableComponent
{
public:

private:
	float _lookSpeed = 1.0f;
	float _moveSpeed = 1.0f;

public:
	PlayerController() {};
	~PlayerController() {};
	void Init() override;
	void Update(float deltaTime) override;

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<ScriptableComponent>(this));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<ScriptableComponent>(this));
	};

};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(PlayerController);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, PlayerController);

CEREAL_FORCE_DYNAMIC_INIT(PlayerController)