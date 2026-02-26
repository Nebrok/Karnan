#pragma once

#include "ScriptComponent.h"


class KeySystem : public ScriptableComponent
{
public:


private:
	bool _hasKey = false;

public:
	KeySystem();
	~KeySystem();


	void Init() override;
	void Start() override;
	void Update(float deltaTime) override;
	void ImGuiRender() override;

	bool GetHasKey() { return _hasKey; };
	void SetHasKey(bool hasKey) { _hasKey = hasKey; };

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

CEREAL_REGISTER_TYPE(KeySystem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, KeySystem);

CEREAL_FORCE_DYNAMIC_INIT(KeySystem)