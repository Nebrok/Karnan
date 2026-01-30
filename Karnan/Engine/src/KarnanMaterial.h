#pragma once

//#include "SimpleRenderSystem.h"

#include "KarnanDevice.h"
#include "VulkanDescriptors.h"
#include "KarnanTextures.h"

#include <string>
#include <vector>
#include <memory>


struct MaterialConstructParams
{
	std::string MaterialName;
	std::string Textures[8];
};


class KarnanMaterial
{
public:


private:
	const uint32_t MAX_TEXTURES = 8;

	KarnanDevice& _karnanDevice;

	std::vector<std::shared_ptr<KarnanTexture>> _materialTextures;
	std::vector<VkDescriptorImageInfo> _materialDescriptorImages;

	VkDescriptorSet _materialDescriptorSet;
	bool _hasMaterialDescriptorSet = false;

	VkFilter _minFilter = VK_FILTER_LINEAR;
	VkFilter _magFilter = VK_FILTER_LINEAR;
	VkSamplerAddressMode _samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	VkSampler _sampler;


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

	void SetMinFilter(VkFilter filter) { _minFilter = filter; };
	void SetMagFilter(VkFilter filter) { _magFilter = filter; };
	void SetAddressMode(VkSamplerAddressMode addressMode) { _samplerAddressMode = addressMode; };


private:


};

