#include "BasicMesh.h"

//std libs
#include <vector>



BasicMesh::BasicMesh(KarnanDevice& device)
	: _karnanDevice(device), _vertexBuffer(device)
{
	VertexBuffer::Vertex vertex1{};
	vertex1.position = { 0.f, -0.5f };
	VertexBuffer::Vertex vertex2{};
	vertex2.position = { 0.5f, 0.5f };
	VertexBuffer::Vertex vertex3{};
	vertex3.position = { -0.5f, 0.5f };

	std::vector<VertexBuffer::Vertex> vertices
	{
		vertex1,
		vertex2,
		vertex3
	};

	_vertexBuffer.CreateVertexBuffers(vertices);
}

BasicMesh::~BasicMesh()
{
	if (_hasIndexBuffer)
	{
		vkDestroyBuffer(_karnanDevice.Device(), _indexBuffer, nullptr);
		vkFreeMemory(_karnanDevice.Device(), _indexBufferMemory, nullptr);
	}
}

void BasicMesh::Bind(VkCommandBuffer commandBuffer)
{
	_vertexBuffer.Bind(commandBuffer);

	if (_hasIndexBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
}

void BasicMesh::Draw(VkCommandBuffer commandBuffer)
{
	if (_hasIndexBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(commandBuffer, _vertexBuffer.GetVertexCount(), 1, 0, 0);
	}
}

void BasicMesh::CreateIndexBuffers(const std::vector<uint32_t>& indices)
{
	_indexCount = static_cast<uint32_t>(indices.size());
	_hasIndexBuffer = _indexCount > 0;

	if (!_hasIndexBuffer)
		return;

	VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	_karnanDevice.CreateBuffer(bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(_karnanDevice.Device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(_karnanDevice.Device(), stagingBufferMemory);

	_karnanDevice.CreateBuffer(bufferSize,	VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,_indexBuffer,_indexBufferMemory);

	_karnanDevice.CopyBuffer(stagingBuffer, _indexBuffer, bufferSize);

	vkDestroyBuffer(_karnanDevice.Device(), stagingBuffer, nullptr);
	vkFreeMemory(_karnanDevice.Device(), stagingBufferMemory, nullptr);
}
