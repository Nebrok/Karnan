#include "VertexBuffer.h"

//#include "KarnanUtils.h"

// std libs
#include <stdexcept>



std::vector<VkVertexInputBindingDescription> VertexBuffer::Vertex::GetBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> VertexBuffer::Vertex::GetAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	attributeDescriptions.push_back({ 0,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
	attributeDescriptions.push_back({ 1,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	attributeDescriptions.push_back({ 2,0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

	return attributeDescriptions;
}


VertexBuffer::VertexBuffer(KarnanDevice& device)
	:_karnanDevice(device)
{

}

VertexBuffer::VertexBuffer(KarnanDevice& device, const std::vector<VertexBuffer::Vertex>& vertices)
	: _karnanDevice(device)
{
	CreateVertexBuffers(vertices);
}

VertexBuffer::~VertexBuffer()
{
	if (_vertexBuffer == nullptr)
		return;

	vkDestroyBuffer(_karnanDevice.Device(), _vertexBuffer, nullptr);
	vkFreeMemory(_karnanDevice.Device(), _vertexBufferMemory, nullptr);
}

void VertexBuffer::Bind(VkCommandBuffer commandBuffer)
{
	if (_vertexBuffer == nullptr)
	{
		throw std::runtime_error("Can't bind VertexBuffer if buffer not created!");
	}

	VkBuffer buffers[] = { _vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void VertexBuffer::CreateVertexBuffers(const std::vector<Vertex>& vertices)
{
	_vertexCount = static_cast<uint32_t>(vertices.size());
	assert(_vertexCount >= 3 && "Vertex count must be at least 3");
	VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	_karnanDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);



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
