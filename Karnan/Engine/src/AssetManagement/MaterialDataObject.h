#pragma once

#include <string>

class MaterialDataObject
{

public:
	std::string MaterialName = "test";
	std::string Textures[8];
	std::string Filepath;

private:


public:
	MaterialDataObject();
	~MaterialDataObject();

	std::string SaveMaterial();
	void LoadMaterial(std::string filePath);

	void UpdateData(MaterialDataObject& newData);


private:


};