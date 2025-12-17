#pragma once

#include "../GameObject.h"

class PointLight : public GameObject
{

public:



private:
	glm::vec3 _lightColour{ 1.0f, 1.0f, 1.0f };
	glm::vec3 _attenuation{ 1.0f, 0.1f, 0.01f };
	float _intensity = 1.0f;


public:
	PointLight();


	void Init() override;
	void Update(double deltaTime) override;

	glm::vec3 GetColour() {	return _lightColour;};
	glm::vec3 GetAttentuation() { return _attenuation; };
	float GetIntensity() { return _intensity; };

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<GameObject>(this));
		ar(CEREAL_NVP(_lightColour));
		ar(CEREAL_NVP(_attenuation));
		ar(CEREAL_NVP(_intensity));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<GameObject>(this));
		ar(CEREAL_NVP(_lightColour));
		ar(CEREAL_NVP(_attenuation));
		ar(CEREAL_NVP(_intensity));
	};

private:
	


};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(PointLight);
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObject, PointLight);