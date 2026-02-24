#pragma once

#include "../GameObject.h"

class TerrainObject : public GameObject
{
public:


private:

	//std::string _filepath = "assets/textures/Gradient.png";
	std::string _heightMapFilepath = "assets/textures/Level1.png";
	//std::string _filepath = "assets/textures/botwTerrainHeightMap.png";


	float _maxHeight = 6.0f;
	float _length = 20.0f;
	float _width = 20.0f;

	int _textureWidth;
	int _textureHeight;


	std::vector<float> _heights;

public:
    TerrainObject();
    ~TerrainObject();

    void Init() override;
	void Start() override;
    void Update(double deltaTime) override;

	bool InTerrainBounds(glm::vec2& coordinates);
	float HeightAt(glm::vec2& coordinates);


	std::string GetHeightMapFilepath() { return _heightMapFilepath; };
	void SetHeightMapFilepath(std::string filepath); 

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
	void GenerateTerrain();

	float InverseLerp(float x, float a, float b);

	std::string GetFilenameFromFilepath();

};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(TerrainObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObject, TerrainObject);