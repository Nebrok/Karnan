#include "MaterialDataObject.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#define MATERIAL_FILE_VERSION "0.1"


MaterialDataObject::MaterialDataObject()
{

}

MaterialDataObject::~MaterialDataObject()
{

}

std::string MaterialDataObject::SaveMaterial()
{
	std::fstream fileOut;
	std::string materialFolder = "./assets/materials/";
	std::string filename = MaterialName + ".kmat";
	fileOut.open(materialFolder + filename, std::fstream::out);
	if (fileOut.fail())
	{
		std::cout << "Failed to open file: " << materialFolder + filename << '\n';
	}

	std::string version = MATERIAL_FILE_VERSION;
	version.append("\n");
	fileOut.write(version.c_str(), version.size());

	std::string materialNameLine = "Name: " + MaterialName + "\n";
	fileOut.write(materialNameLine.c_str(), materialNameLine.size());

	for (int i = 0; i < 8; i++)
	{
		std::string textureNameLine = "Texture " + std::to_string(i) + ": " + Textures[i] + "\n";
		fileOut.write(textureNameLine.c_str(), textureNameLine.size());
	}

	fileOut.close();
	return materialFolder + filename;
}

void MaterialDataObject::LoadMaterial(std::string filePath)
{
	//TODO
}

