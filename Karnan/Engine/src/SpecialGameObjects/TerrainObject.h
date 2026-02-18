#pragma once

#include "../GameObject.h"

class TerrainObject : public GameObject
{
public:


private:

	std::string _filepath = "assets/textures/botwTerrainHeightMaplow.png";


	float _maxHeight = 100.0f;
	float _length = 1000.0f;
	float _width = 1000.0f;

public:
    TerrainObject();
    ~TerrainObject();

    void Init() override;
    void Update(double deltaTime) override;



	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<GameObject>(this));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<GameObject>(this));
	};

private:


};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(TerrainObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObject, TerrainObject);