#pragma once

#include <string>

class MaterialDataObject
{

public:
	std::string MaterialName = "test";
	std::string Textures[8];

private:


public:
	MaterialDataObject();
	~MaterialDataObject();

	std::string SaveMaterial();
	void LoadMaterial(std::string filePath);



private:


};