#pragma once

#include "KarnanDevice.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BasicMesh.h"
//std libs
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>


class MeshLoadingSystem
{
public:
	static MeshLoadingSystem* Instance;

private:
	KarnanDevice& _karnanDevice;


	std::unordered_map<std::string, std::shared_ptr<VertexBuffer>> _vertexBufferMap;
	std::unordered_map<std::string, std::shared_ptr<IndexBuffer>> _indexBufferMap;

	std::unordered_map<std::string, std::shared_ptr<BasicMesh>> _meshMap;


public:
	static MeshLoadingSystem* StartMeshLoadingSystem();
	static void DestroyMeshLoadingSystem();


	void LoadMesh(const std::string& filename);
	
	void CreateMesh(const std::string& modelName, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);
	
	std::shared_ptr<VertexBuffer> GetVertexBuffer(const std::string& filename);
	std::shared_ptr<IndexBuffer> GetIndexBuffer(const std::string& filename);

private:
	MeshLoadingSystem();

	void SerialiseMesh(const std::string& outputPath, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);
	void LoadModelFromKMSH(const std::string& filepath, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);

	std::string CheckForBinary(const std::string& filename);
	void LoadObj(const std::string& filename);
	std::vector<std::string> Triangularise(const std::vector<std::string>& polygon);
	std::vector<int> ParseFacePoint(const std::string& point);
};