#pragma once

#include "ScriptComponent.h"


class Key : public ScriptableComponent
{
public:


private:


public:
	Key();
	~Key();


	void Init() override;
	void Start() override;
	void Update(float deltaTime) override;
	void ImGuiRender() override;


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

private:


};


#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(Key);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, Key);

CEREAL_FORCE_DYNAMIC_INIT(Key)