#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "KarnanDevice.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

//std libs
#include <memory>


class BasicMesh
{
public:

private:
	KarnanDevice& _karnanDevice;

	std::unique_ptr<VertexBuffer> _vertexBuffer;
	std::unique_ptr<IndexBuffer> _indexBuffer;

	bool _hasIndexBuffer = false;



public:
	BasicMesh(KarnanDevice& device, std::vector<VertexBuffer::Vertex> vertices, std::vector<uint32_t> indices);
	~BasicMesh();

	void CreateMesh(std::vector<VertexBuffer::Vertex> vertices, std::vector<uint32_t> indices);

	void Bind(VkCommandBuffer commandBuffer);
	void Draw(VkCommandBuffer commandBuffer);

private:


};