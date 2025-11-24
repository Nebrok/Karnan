#include "MeshLoadingSystem.h"

#include "EngineCore.h"
#include "KarnanUtils.h"

//std libs
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

/*
namespace std
{
	template<>
	struct hash<VertexBuffer::Vertex>
	{
		size_t operator()(VertexBuffer::Vertex const& vertex) const
		{
			size_t seed = 0;
			Karnan::HashCombine(seed, vertex.position, vertex.normal, vertex.uv);
			return seed;
		}
	};
}
*/

MeshLoadingSystem* MeshLoadingSystem::Instance = nullptr;

MeshLoadingSystem* MeshLoadingSystem::StartMeshLoadingSystem()
{
	if (MeshLoadingSystem::Instance == nullptr)
	{
		Instance = DBG_NEW MeshLoadingSystem();
	}
	return Instance;
}

void MeshLoadingSystem::DestroyMeshLoadingSystem()
{
	delete(MeshLoadingSystem::Instance);
}

void MeshLoadingSystem::LoadMesh(const std::string& filename)
{
	if (auto search = _vertexBufferMap.find(filename); search != _vertexBufferMap.end())
	{
		std::cout << "Model: " << filename << " is already loaded.";
	}
	else
	{
		LoadObj(filename);
	}
}

void MeshLoadingSystem::LoadObj(const std::string& filename)
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
	int lineCount = 0;

	//std::vector<> uniqueVertices{};

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

			bool redOut = false;
			if (points.size() > 4) // Ngons/Polygons
			{
				points = Triangularise(points);
			}
			else if (points.size() == 4) //Quads
			{
				points = Triangularise(points);
			}

			for (int i = 0; i < points.size(); i++)
			{
				std::vector<int> pointIndices = ParseFacePoint(points[i]);

				for (int index : pointIndices)
				{
					if (index < 0)
						throw std::runtime_error("Negative indexes in obj not accounted for yet.");
				}

				VertexBuffer::Vertex vertex;
				vertex.position = modelVertices[pointIndices[0] - 1];
				if (pointIndices[1] > 0)
					vertex.uv = modelUVs[pointIndices[1] - 1];
				else
					vertex.uv = { 0,0 };

				if (redOut)
					vertex.normal = { 1.0f, 0.0f, 0.0f };
				else
					vertex.normal = modelNormals[pointIndices[2] - 1];



				/*
				int outIndex = Contains(vertices, vertex);
				if (outIndex == -1)
				{
					vertices.push_back(vertex);
				}
				indices.push_back(outIndex);
				*/

				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}

		}
		lineCount++;
	}
	file.close();
	std::cout << filename << " loaded succesfully with: " << '\n';
	std::cout << "Vertices: " << vertices.size() << '\n';
	std::cout << "Indices: " << indices.size() << '\n';
	CreateMesh(filename, vertices, indices);
}

void MeshLoadingSystem::CreateMesh(const std::string& modelName, std::vector<VertexBuffer::Vertex> vertices, std::vector<uint32_t> indices)
{
	std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(_karnanDevice));
	_vertexBufferMap.insert({ modelName, vertexBuffer });
	vertexBuffer->CreateVertexBuffers(vertices);

	if (indices.size() > 0)
	{
		std::shared_ptr<IndexBuffer> indexBuffer(DBG_NEW IndexBuffer(_karnanDevice));
		_indexBufferMap.insert({ modelName, indexBuffer });
		indexBuffer->CreateIndexBuffers(indices);
	}
}

std::shared_ptr<VertexBuffer> MeshLoadingSystem::GetVertexBuffer(const std::string& filename)
{
	if (auto search = _vertexBufferMap.find(filename); search != _vertexBufferMap.end())
	{
		return _vertexBufferMap.at(filename);
	}
	return nullptr;
}

std::shared_ptr<IndexBuffer> MeshLoadingSystem::GetIndexBuffer(const std::string& filename)
{
	if (auto search = _indexBufferMap.find(filename); search != _indexBufferMap.end())
	{
		return _indexBufferMap.at(filename);
	}
	return nullptr;
}

MeshLoadingSystem::MeshLoadingSystem()
	: _karnanDevice(EngineCore::Instance->Device())
{

}

std::vector<std::string> MeshLoadingSystem::Triangularise(const std::vector<std::string>& polygon)
{
	std::vector<std::string> expanded;
	if (polygon.size() < 5)
	{
		expanded.push_back(polygon[0]);
		expanded.push_back(polygon[1]);
		expanded.push_back(polygon[3]);

		expanded.push_back(polygon[1]);
		expanded.push_back(polygon[2]);
		expanded.push_back(polygon[3]);
	}
	else
	{
		for (int i = 1; i < polygon.size() - 1; i++)
		{
			expanded.push_back(polygon[0]);
			expanded.push_back(polygon[i]);
			expanded.push_back(polygon[i + 1]);
		}
	}

	return expanded;
}

std::vector<int> MeshLoadingSystem::ParseFacePoint(const std::string& point)
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

int MeshLoadingSystem::Contains(std::vector<VertexBuffer::Vertex> vertices, VertexBuffer::Vertex comparison)
{
	size_t index = 0;
	for (auto vertex : vertices)
	{
		if (vertex == comparison)
		{
			return index;
		}
		index++;
	}
	return -1;
}
