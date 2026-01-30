#include "MaterialDataObject.h"

#include "AssetManager.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#define MATERIAL_FILE_VERSION "0.2"


MaterialDataObject::MaterialDataObject()
{

}

MaterialDataObject::~MaterialDataObject()
{

}

std::string MaterialDataObject::SaveMaterial()
{
	std::fstream fileOut;
	std::string materialFolder = "assets/materials/";
	std::string filename = MaterialName + ".kmat";
	fileOut.open(materialFolder + filename, std::fstream::out);
	if (fileOut.fail())
	{
		std::cout << "Failed to save material data file: " << materialFolder + filename << '\n';
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

	std::string minFilterLine = "MinFilter: " + FilterToString(MinFilter) + "\n";
	fileOut.write(minFilterLine.c_str(), minFilterLine.size());

	std::string magFilterLine = "MagFilter: " + FilterToString(MagFilter) + "\n";
	fileOut.write(magFilterLine.c_str(), magFilterLine.size());

	std::string addressModeLine = "AddressMode: " + AddressModeToString(SamplerAddressMode) + "\n";
	fileOut.write(addressModeLine.c_str(), addressModeLine.size());

	fileOut.close();
	return materialFolder + filename;
}

void MaterialDataObject::LoadMaterial(std::string filePath)
{
	std::fstream fileIn;
	fileIn.open(filePath, std::fstream::in);
	if (fileIn.fail())
	{
		std::cout << "Failed to open material data file: " << filePath << '\n';
	}


	int lineCount = 0;

	std::string line;
	while (std::getline(fileIn, line))
	{
		std::istringstream iss(line);
		if (lineCount == 0)
		{
			if (line != MATERIAL_FILE_VERSION)
				throw std::runtime_error("Trying to open old material data version");
			lineCount++;
			continue;
		}
		std::vector<std::string> splitLine;
		std::string word;
		while (std::getline(iss, word, ' '))
		{
			splitLine.push_back(word);
		}

		if (splitLine[0] == "Name:")
		{
			//+1 for space
			MaterialName = line.substr(splitLine[0].size() + 1);
		}

		if (splitLine[0] == "Texture")
		{
			int index = splitLine[1].at(0) - '0';
			Textures[index] = line.substr(splitLine[0].size() + 1 + splitLine[1].size() + 1);
		}

		if (splitLine[0] == "MinFilter:")
		{
			std::string filterName = splitLine[1];
			MinFilter = FilterNameToEnum(filterName);
		}

		if (splitLine[0] == "MagFilter:")
		{
			std::string filterName = splitLine[1];
			MagFilter = FilterNameToEnum(filterName);
		}

		if (splitLine[0] == "AddressMode:")
		{
			std::string samplerAddressModeName = splitLine[1];
			SamplerAddressMode = AddressModeNametoEnum(samplerAddressModeName);
		}

		lineCount++;
	}

	fileIn.close();
	Filepath = filePath;
}

void MaterialDataObject::UpdateData(MaterialDataObject& newData)
{
	std::string oldFilepath = Filepath;
	std::string oldMaterialName = MaterialName;

	MaterialName = newData.MaterialName;

	for (int i = 0; i < 8; i++)
	{
		Textures[i] = newData.Textures[i];
	}

	MinFilter = newData.MinFilter;
	MagFilter = newData.MagFilter;
	SamplerAddressMode = newData.SamplerAddressMode;

	std::string newFilepath = SaveMaterial();
	Filepath = newFilepath;

	if (oldMaterialName != newData.MaterialName)
	{
		AssetManager::Instance->UpdateMaterialDataMap(oldFilepath);
		std::filesystem::remove(oldFilepath);
	}


}

std::string MaterialDataObject::FilterToString(VkFilter filter)
{
	switch (filter)
	{
	case VkFilter::VK_FILTER_LINEAR:
		return std::string("VK_FILTER_LINEAR");
	case VkFilter::VK_FILTER_NEAREST:
		return std::string("VK_FILTER_NEAREST");
	default:
		return std::string("INVALID");
	}
}

std::string MaterialDataObject::AddressModeToString(VkSamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT:
		return std::string("VK_SAMPLER_ADDRESS_MODE_REPEAT");

	case VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
		return std::string("VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT");

	case VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
		return std::string("VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE");

	case VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
		return std::string("VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER");

	case VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
		return std::string("VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE");
	
	default:
		return std::string("INVALID");
	}
}

VkFilter MaterialDataObject::FilterNameToEnum(const std::string& filterName)
{
	if (filterName.compare("VK_FILTER_LINEAR") == 0)
		return VkFilter::VK_FILTER_LINEAR;
	else if (filterName.compare("VK_FILTER_NEAREST") == 0)
		return VkFilter::VK_FILTER_NEAREST;

	return VkFilter::VK_FILTER_LINEAR;
}

VkSamplerAddressMode MaterialDataObject::AddressModeNametoEnum(const std::string& addressModeName)
{
	if (addressModeName.compare("VK_SAMPLER_ADDRESS_MODE_REPEAT") == 0)
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
	else if (addressModeName.compare("VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT") == 0)
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	else if (addressModeName.compare("VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE") == 0)
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	else if (addressModeName.compare("VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER") == 0)
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	else if (addressModeName.compare("VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE") == 0)
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

	return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

