#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "KarnanDevice.h"


class BasicMesh
{
public:
	struct Vertex 
	{
		glm::vec2 position{};
		glm::vec3 normal{};

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

		bool operator==(const Vertex& other) const
		{
			return position == other.position && normal == other.normal;
		}
	};


private:
	KarnanDevice& _karnanDevice;

	VkBuffer _vertexBuffer;
	VkDeviceMemory _vertexBufferMemory;
	uint32_t _vertexCount;


	bool _hasIndexBuffer = false;
	VkBuffer _indexBuffer;
	VkDeviceMemory _indexBufferMemory;
	uint32_t _indexCount;


public:
	BasicMesh(KarnanDevice& device);
	~BasicMesh();

	void Bind(VkCommandBuffer commandBuffer);
	void Draw(VkCommandBuffer commandBuffer);

private:
	void CreateVertexBuffers(const std::vector<Vertex>& vertices);
	void CreateIndexBuffers(const std::vector<uint32_t>& indices);


};