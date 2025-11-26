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
		//IF THIS IS CHANGED AT ALL THERE IS NOW A CASCADE OF SYSTEM THAT NEED TO CHANGE
		//PRIMARILY MESH LOADING
		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

		bool operator==(const Vertex& other) const
		{
			return position == other.position && uv == other.uv && normal == other.normal;
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