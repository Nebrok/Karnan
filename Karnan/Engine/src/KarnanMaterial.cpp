#include "KarnanMaterial.h"

#include <stdexcept>

#include "EngineCore.h"

KarnanMaterial::KarnanMaterial()
	: _karnanDevice(EngineCore::Device())
{
	_materialTextures.resize(MAX_TEXTURES);
	_materialDescriptorSet = {};
}

KarnanMaterial::~KarnanMaterial()
{
	vkDestroySampler(_karnanDevice.Device(), _sampler, nullptr);
}

void KarnanMaterial::Init()
{
	CreateSampler();

}

void KarnanMaterial::CreateSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = nullptr;
	samplerInfo.magFilter = _magFilter;
	samplerInfo.minFilter = _minFilter;
	samplerInfo.addressModeU = _samplerAddressMode;
	samplerInfo.addressModeV = _samplerAddressMode;
	samplerInfo.addressModeW = _samplerAddressMode;

	vkCreateSampler(_karnanDevice.Device(), &samplerInfo, nullptr, &_sampler);
}

void KarnanMaterial::CreateImageInfos()
{
	_materialDescriptorImages.clear();
	for (int i = 0; i < _materialTextures.size(); i++)
	{
		if (_materialTextures[i] == nullptr)
			continue;

		VkDescriptorImageInfo imageBufferInfo{};
		imageBufferInfo.sampler = _sampler;
		imageBufferInfo.imageView = (_materialTextures[i])->GetImageView();
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		_materialDescriptorImages.push_back(imageBufferInfo);
	}
}

void KarnanMaterial::CreateTextureInSlot(uint32_t slot, std::string filePath)
{
	if (slot >= MAX_TEXTURES)
		throw std::runtime_error("Tried to create texture in invalid slot on material");

	if (_materialTextures[slot] != nullptr)
	{
		throw std::runtime_error("Texture already exists in slot that the texture is being bound to");
	}

	if (AssetManager::Instance->IsTextureLoaded(filePath))
	{
		_materialTextures[slot] = AssetManager::Instance->GetTexture(filePath);
	}
	else
	{
		AssetManager::Instance->CreateTexture(filePath);
		_materialTextures[slot] = AssetManager::Instance->GetTexture(filePath);
	}	
}

