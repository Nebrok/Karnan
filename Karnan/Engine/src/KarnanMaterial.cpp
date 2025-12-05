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
	vkDestroySampler(_karnanDevice.Device(), _tempSampler, nullptr);
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
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	vkCreateSampler(_karnanDevice.Device(), &samplerInfo, nullptr, &_tempSampler);
}

void KarnanMaterial::CreateImageInfos()
{
	_materialDescriptorImages.clear();
	for (int i = 0; i < _materialTextures.size(); i++)
	{
		if (_materialTextures[i] == nullptr)
			continue;

		VkDescriptorImageInfo imageBufferInfo{};
		imageBufferInfo.sampler = _tempSampler;
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

	std::unique_ptr<KarnanTexture> texture(DBG_NEW KarnanTexture(filePath));
	_materialTextures[slot] = move(texture);
}

