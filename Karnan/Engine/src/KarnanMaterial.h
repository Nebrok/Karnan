#pragma once

//#include "SimpleRenderSystem.h"

#include "KarnanDevice.h"
#include "VulkanDescriptors.h"
#include "KarnanTextures.h"


#include <string>
#include <vector>
#include <memory>

class KarnanMaterial
{
public:


private:
	const uint32_t MAX_TEXTURES = 8;

	KarnanDevice& _karnanDevice;

	std::string _vertexShaderFilepath;
	std::string _fragShaderFilepath;

	std::vector<std::unique_ptr<KarnanTexture>> _materialTextures;
	std::vector<VkDescriptorImageInfo> _materialDescriptorImages;

	VkDescriptorSet _materialDescriptorSet;
	bool _hasMaterialDescriptorSet = false;

	VkSampler _tempSampler;

public:
	KarnanMaterial();
	~KarnanMaterial();

	void Init();

	void CreateSampler();
	void CreateImageInfos();

	void CreateTextureInSlot(uint32_t slot, std::string filePath);

	VkDescriptorImageInfo& GetImageInfosAtIndex(int index) { return _materialDescriptorImages[index]; };
	bool IsTextureBoundAt(int index) { return _materialTextures[index] != nullptr; };
	size_t GetTotalTextures() { return _materialTextures.size(); };

	VkDescriptorSet& GetDescriptorSet() { return _materialDescriptorSet; };
	bool HasDescriptorSet() const { return _hasMaterialDescriptorSet; };
	void SetValidDescriptorSet() { _hasMaterialDescriptorSet = true; };
private:


};

