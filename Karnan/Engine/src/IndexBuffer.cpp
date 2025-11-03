#include "IndexBuffer.h"

// std libs
#include <stdexcept>

IndexBuffer::IndexBuffer(KarnanDevice& device)
	: _karnanDevice(device)
{
}

IndexBuffer::IndexBuffer(KarnanDevice& device, const std::vector<uint32_t>& indices)
	: _karnanDevice(device)
{
	CreateIndexBuffers(indices);
}

IndexBuffer::~IndexBuffer()
{
	if (_indexBuffer == nullptr)
		return;

	vkDestroyBuffer(_karnanDevice.Device(), _indexBuffer, nullptr);
	vkFreeMemory(_karnanDevice.Device(), _indexBufferMemory, nullptr);
}

void IndexBuffer::CreateIndexBuffers(const std::vector<uint32_t>& indices)
{
	_indexCount = static_cast<uint32_t>(indices.size());

	VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	_karnanDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(_karnanDevice.Device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(_karnanDevice.Device(), stagingBufferMemory);

	_karnanDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

	_karnanDevice.CopyBuffer(stagingBuffer, _indexBuffer, bufferSize);

	vkDestroyBuffer(_karnanDevice.Device(), stagingBuffer, nullptr);
	vkFreeMemory(_karnanDevice.Device(), stagingBufferMemory, nullptr);
}

void IndexBuffer::Bind(VkCommandBuffer commandBuffer)
{
	if (_indexBuffer == nullptr)
	{
		throw std::runtime_error("Can't bind VertexBuffer if buffer not created!");
	}

	vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}
