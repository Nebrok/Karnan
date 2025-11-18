#include "BasicMesh.h"

#include "EngineCore.h"

//std libs
#include <vector>
#include <fstream>
#include <sstream>


BasicMesh::BasicMesh(const std::string& filename)
	:_karnanDevice(EngineCore::Instance->Device())
{
	std::unique_ptr<VertexBuffer> vertexBuffer(new VertexBuffer(_karnanDevice));
	_vertexBuffer = move(vertexBuffer);

	std::unique_ptr<IndexBuffer> indexBuffer(new IndexBuffer(_karnanDevice));
	_indexBuffer = move(indexBuffer);
	LoadMesh(filename);
}

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

void BasicMesh::LoadMesh(const std::string& filename)
{
	LoadObj(filename);
}

void BasicMesh::LoadObj(const std::string& filename)
{
	std::vector<VertexBuffer::Vertex> vertices;
	std::vector<uint32_t> indices;

	std::vector<glm::vec3> modelVertices;
	std::vector<glm::vec3> modelNormals;
	std::vector<glm::vec2> modelUVs;

	std::ifstream file(filename);
	std::string line;
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file: " + filename);
	}
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "v")
		{
			glm::vec3 modelVertex;
			iss >> modelVertex.x >> modelVertex.y >> modelVertex.z;
			modelVertices.push_back(modelVertex);
		}

		if (prefix == "vt")
		{
			glm::vec3 modelUV;
			iss >> modelUV.x >> modelUV.y;
			modelUVs.push_back(modelUV);
		}

		if (prefix == "vn")
		{
			glm::vec3 modelNormal;
			iss >> modelNormal.x >> modelNormal.y >> modelNormal.z;
			modelNormals.push_back(modelNormal);
		}

		if (prefix == "f")
		{
			std::vector<std::string> points;
			std::string point;

			while (iss >> point)
			{
				points.push_back(point);
			}

			if (points.size() > 4)
			{
				points = Triangularise(points);
				continue;
			}
			else if (points.size() > 3)
			{
				points = Triangularise(points);
			}

			for (int i = 0; i < points.size(); i++)
			{
				std::vector<int> indices = ParseFacePoint(points[i]);

				for (int index : indices)
				{
					if (index < 0)
						throw std::runtime_error("Negative indexes in obj not accounted for yet.");
				}

				VertexBuffer::Vertex vertex;
				vertex.position = modelVertices[indices[0] - 1];
				if (indices[1] > 0)
					vertex.uv = modelUVs[indices[1] - 1];
				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}

		}
	}
	file.close();
	CreateMesh(vertices, indices);
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

std::vector<std::string> BasicMesh::Triangularise(const std::vector<std::string>& polygon)
{
	std::vector<std::string> expanded;
	if (polygon.size() < 5)
	{
		expanded.push_back(polygon[0]);
		expanded.push_back(polygon[1]);
		expanded.push_back(polygon[2]);
		expanded.push_back(polygon[0]);
		expanded.push_back(polygon[2]);
		expanded.push_back(polygon[3]);
	}
	else
	{
		for (int i = 1; i < polygon.size() - 1; i++)
		{
			expanded.push_back(polygon[0    ]);
			expanded.push_back(polygon[i]);
			expanded.push_back(polygon[i + 1]);
		}
	}
	
	return expanded;
}

std::vector<int> BasicMesh::ParseFacePoint(const std::string& point)
{
	std::vector<int> indices;
	std::istringstream ss(point);
	std::string part;

	// Extract v
	if (std::getline(ss, part, '/'))
		indices.push_back(std::stoi(part));

	// Extract vt
	if (std::getline(ss, part, '/'))
		indices.push_back(part.empty() ? 0 : std::stoi(part));   // vt may be missing

	// Extract vn
	if (std::getline(ss, part, '/'))
		indices.push_back(std::stoi(part));

	return indices;
}
