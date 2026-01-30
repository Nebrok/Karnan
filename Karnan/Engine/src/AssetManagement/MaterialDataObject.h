#pragma once

#include <string>
#include "vulkan/vulkan.h"


class MaterialDataObject
{

public:
	std::string MaterialName = "test";
	std::string Textures[8];
	std::string Filepath;

	VkFilter MinFilter = VK_FILTER_LINEAR;
	VkFilter MagFilter = VK_FILTER_LINEAR;
	VkSamplerAddressMode SamplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

private:


public:
	MaterialDataObject();
	~MaterialDataObject();

	std::string SaveMaterial();
	void LoadMaterial(std::string filePath);

	void UpdateData(MaterialDataObject& newData);

	static VkFilter FilterNameToEnum(const std::string& filterName);
	static VkSamplerAddressMode AddressModeNametoEnum(const std::string& addressModeName);
	static std::string FilterToString(VkFilter filter);
	static std::string AddressModeToString(VkSamplerAddressMode addressMode);

private:

};