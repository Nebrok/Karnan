#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "KarnanDevice.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

//std libs
#include <memory>
#include <string>

class BasicMesh
{
public:

private:
	KarnanDevice& _karnanDevice;

	std::shared_ptr<VertexBuffer> _vertexBuffer = nullptr;
	std::shared_ptr<IndexBuffer> _indexBuffer = nullptr;

	bool _hasIndexBuffer = false;

public:
	BasicMesh(const std::string& filename, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer);
	BasicMesh(KarnanDevice& device, std::string modelName, std::vector<VertexBuffer::Vertex> vertices, std::vector<uint32_t> indices);
	~BasicMesh();

	std::shared_ptr<VertexBuffer> GetVertexBuffer() { return _vertexBuffer; };
	std::shared_ptr<IndexBuffer> GetIndexBuffer() { return _indexBuffer; };

	void Bind(VkCommandBuffer commandBuffer);
	void Draw(VkCommandBuffer commandBuffer);

private:

};