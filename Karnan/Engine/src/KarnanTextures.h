#pragma once

#include "KarnanDevice.h"

#include <string>
#include <../stb/stb_image.h>

class KarnanTexture
{
public:

private:
	KarnanDevice& _karnanDevice;

	VkImage _textureImage;
	VkDeviceMemory _textureImageMemory;

	VkImageView _textureImageView;

	std::string _filepath;


public:
	KarnanTexture(std::string filepath);
	~KarnanTexture();

	KarnanTexture(const KarnanTexture&) = delete;
	KarnanTexture& operator=(const KarnanTexture&) = delete;

	VkImageView& GetImageView() { return _textureImageView; };


private:
	void CreateTextureImage();
	void CreateImage(uint32_t width, uint32_t height, 
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


	void CreateTextureImageView();

};