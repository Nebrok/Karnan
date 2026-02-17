#pragma once

#include "../GameObject.h"

class TerrainObject : public GameObject
{
public:


private:

	float _maxHeight;


public:
    TerrainObject();
    ~TerrainObject();

    void Init() override;
    void Update(double deltaTime) override;
    void Render(VkCommandBuffer commandBuffer) override;



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