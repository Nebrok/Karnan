#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


#include "KarnanDevice.h"

class VertexBuffer 
{
public:
	struct Vertex
	{
		glm::vec2 position{};

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

		bool operator==(const Vertex& other) const
		{
			return position == other.position;
		}
	};

private:
	KarnanDevice& _karnanDevice;

	VkBuffer _vertexBuffer;
	VkDeviceMemory _vertexBufferMemory;
	uint32_t _vertexCount;

public:
	VertexBuffer(KarnanDevice& device);
	VertexBuffer(KarnanDevice& device, const std::vector<VertexBuffer::Vertex>& vertices);
	~VertexBuffer();

	void CreateVertexBuffers(const std::vector<Vertex>& vertices);

	uint32_t GetVertexCount() const { return _vertexCount; }

	void Bind(VkCommandBuffer commandBuffer);


private:


};