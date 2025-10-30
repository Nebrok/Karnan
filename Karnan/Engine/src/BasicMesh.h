#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "KarnanDevice.h"
#include "VertexBuffer.h"


class BasicMesh
{
public:

private:
	KarnanDevice& _karnanDevice;

	VertexBuffer _vertexBuffer;

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
	void CreateIndexBuffers(const std::vector<uint32_t>& indices);


};