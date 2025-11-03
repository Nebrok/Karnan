#include "BasicMesh.h"

//std libs
#include <vector>


BasicMesh::BasicMesh(KarnanDevice& device, std::vector<VertexBuffer::Vertex> vertices, std::vector<uint32_t> indices)
	: _karnanDevice(device)
{
	std::unique_ptr<VertexBuffer> vertexBuffer(new VertexBuffer(device));
	_vertexBuffer = move(vertexBuffer);

	std::unique_ptr<IndexBuffer> indexBuffer(new IndexBuffer(device));
	_indexBuffer = move(indexBuffer);

	CreateMesh(vertices, indices);
}

BasicMesh::~BasicMesh()
{

}

void BasicMesh::CreateMesh(std::vector<VertexBuffer::Vertex> vertices, std::vector<uint32_t> indices)
{
	_vertexBuffer->CreateVertexBuffers(vertices);
	if (indices.size() > 0)
	{
		_indexBuffer->CreateIndexBuffers(indices);
		_hasIndexBuffer = true;
	}
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