#include "BasicMesh.h"

#include "EngineCore.h"
#include "MeshLoadingSystem.h"
#include "KarnanUtils.h"


//std libs
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

BasicMesh::BasicMesh(const std::string& filename, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer)
	:_karnanDevice(EngineCore::Instance->Device())
{
	_vertexBuffer = vertexBuffer;
	if (indexBuffer != nullptr)
	{
		_indexBuffer = indexBuffer;
		_hasIndexBuffer = true;
	}
}

BasicMesh::BasicMesh(KarnanDevice& device, std::string modelName, std::vector<VertexBuffer::Vertex> vertices, std::vector<uint32_t> indices)
	: _karnanDevice(device)
{
	
}

BasicMesh::~BasicMesh()
{

}
void BasicMesh::Bind(VkCommandBuffer commandBuffer)
{
	_vertexBuffer->Bind(commandBuffer);

	if (_hasIndexBuffer)
	{
		_indexBuffer->Bind(commandBuffer);
	}
}

void BasicMesh::Draw(VkCommandBuffer commandBuffer)
{
	if (_hasIndexBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, _indexBuffer->GetIndexCount(), 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(commandBuffer, _vertexBuffer->GetVertexCount(), 1, 0, 0);
	}
}

