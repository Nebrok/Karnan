#pragma once

#include "ScriptComponent.h"


class Door : public ScriptableComponent
{
public:


private:


public:
	Door();
	~Door();


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
	void OpenDoor();

};


#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(Door);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, Door);

CEREAL_FORCE_DYNAMIC_INIT(Door)