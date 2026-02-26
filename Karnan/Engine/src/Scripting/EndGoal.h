#pragma once

#include "ScriptComponent.h"


class EndGoal : public ScriptableComponent
{
public:

private:
	std::string _sceneToLoad = "Level2";


public:
	EndGoal();
	~EndGoal();

	void Init() override;
	void Start() override;
	void Update(float deltaTime) override;
	void ImGuiRender() override;

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<ScriptableComponent>(this));
		ar(CEREAL_NVP(_sceneToLoad));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<ScriptableComponent>(this));
		ar(CEREAL_NVP(_sceneToLoad));
	};

private:


};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(EndGoal);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ScriptableComponent, EndGoal);

CEREAL_FORCE_DYNAMIC_INIT(EndGoal)