#pragma once

#include "../GameObject.h"

class TerrainObject : public GameObject
{
public:


private:

	//std::string _heightMapFilepath = "assets/textures/Gradient.png";
	std::string _heightMapFilepath = "assets/textures/Level1.png";
	//std::string _heightMapFilepath = "assets/textures/botwTerrainHeightMap.png";


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

	float GetMaxHeight() { return _maxHeight; };
	void SetMaxHeight(float height) { _maxHeight = height; };

	float GetWidth() { return _width; };
	void SetWidth(float width) { _width = width; };

	float GetLength() { return _length; };
	void SetLength(float length) { _length = length; };


	std::string GetHeightMapFilepath() { return _heightMapFilepath; };
	void SetHeightMapFilepath(std::string filepath); 

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<GameObject>(this));
		ar(cereal::make_nvp("HeightMapFilepath", _heightMapFilepath));
		ar(cereal::make_nvp("MaxHeight", _maxHeight));
		ar(cereal::make_nvp("Length", _length));
		ar(cereal::make_nvp("Height", _width));

	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<GameObject>(this));
		ar(cereal::make_nvp("HeightMapFilepath", _heightMapFilepath));
		ar(cereal::make_nvp("MaxHeight", _maxHeight));
		ar(cereal::make_nvp("Length", _length));
		ar(cereal::make_nvp("Height", _width));
	};

private:
	void GenerateTerrain();
	void GenerateHeightVector();

	float InverseLerp(float x, float a, float b);

	std::string GetFilenameFromFilepath();

};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(TerrainObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObject, TerrainObject);