#pragma once

#include "KarnanDevice.h"

class IndexBuffer
{

public:

private:
	KarnanDevice& _karnanDevice;


	VkBuffer _indexBuffer;
	VkDeviceMemory _indexBufferMemory;
	uint32_t _indexCount;


public:
	IndexBuffer(KarnanDevice& device);
	IndexBuffer(KarnanDevice& device, const std::vector<uint32_t>& indices);
	~IndexBuffer();

	uint32_t GetIndexCount() const { return _indexCount; }

	void CreateIndexBuffers(const std::vector<uint32_t>& indices);

	void Bind(VkCommandBuffer commandBuffer);




private:


};