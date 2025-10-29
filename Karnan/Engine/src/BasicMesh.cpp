#include "BasicMesh.h"

//std libs
#include <vector>

std::vector<VkVertexInputBindingDescription> BasicMesh::Vertex::GetBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> BasicMesh::Vertex::GetAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	attributeDescriptions.push_back({ 0,0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position) });
	attributeDescriptions.push_back({ 2,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });

	return attributeDescriptions;
}

BasicMesh::BasicMesh(KarnanDevice& device)
	: _karnanDevice(device)
{
	Vertex vertex1{};
	vertex1.position = { 0.f, -0.5f };
	vertex1.normal = { 0.f, 0.f, 0.f };
	Vertex vertex2{};
	vertex1.position = { 0.5f, 0.5f };
	vertex2.normal = { 0.f, 0.f, 0.f };
	Vertex vertex3{};
	vertex1.position = { -0.5f, 0.5f };
	vertex3.normal = { 0.f, 0.f, 0.f };

	std::vector<BasicMesh::Vertex> vertices
	{
		vertex1,
		vertex2,
		vertex3
	};


	CreateVertexBuffers(vertices);
}

BasicMesh::~BasicMesh()
{
	vkDestroyBuffer(_karnanDevice.Device(), _vertexBuffer, nullptr);
	vkFreeMemory(_karnanDevice.Device(), _vertexBufferMemory, nullptr);

	if (_hasIndexBuffer)
	{
		vkDestroyBuffer(_karnanDevice.Device(), _indexBuffer, nullptr);
		vkFreeMemory(_karnanDevice.Device(), _indexBufferMemory, nullptr);
	}
}

void BasicMesh::Bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { _vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

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
		vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
	}
}

void BasicMesh::CreateVertexBuffers(const std::vector<Vertex>& vertices)
{
	_vertexCount = static_cast<uint32_t>(vertices.size());
	assert(_vertexCount >= 3 && "Vertex count must be at least 3");
	VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	_karnanDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,	stagingBuffer, stagingBufferMemory);



	void* data;
	vkMapMemory(_karnanDevice.Device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(_karnanDevice.Device(), stagingBufferMemory);

	_karnanDevice.CreateBuffer(
		bufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		_vertexBuffer,
		_vertexBufferMemory);

	_karnanDevice.CopyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

	vkDestroyBuffer(_karnanDevice.Device(), stagingBuffer, nullptr);
	vkFreeMemory(_karnanDevice.Device(), stagingBufferMemory, nullptr);
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
