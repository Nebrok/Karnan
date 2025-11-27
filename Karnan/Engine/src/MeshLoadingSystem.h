#pragma once

#include "KarnanDevice.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BasicMesh.h"
#include "MessagingSystem/MessageSystem.h"

//std libs
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>


class MeshLoadingSystem : public IMessageSystem
{
public:
	static MeshLoadingSystem* Instance;

private:
	KarnanDevice& _karnanDevice;

	std::unordered_map<std::string, std::shared_ptr<BasicMesh>> _meshMap;


public:
	static MeshLoadingSystem* StartMeshLoadingSystem();
	static void DestroyMeshLoadingSystem();

	void LoadMesh(const std::string& filename);
	void CreateMesh(const std::string& modelName, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);
	
	std::shared_ptr<BasicMesh> GetMesh(const std::string& filename);
	std::shared_ptr<VertexBuffer> GetVertexBuffer(const std::string& filename);
	std::shared_ptr<IndexBuffer> GetIndexBuffer(const std::string& filename);

	void Processor();

	virtual void QueueMessage(std::shared_ptr<Message> message) override;

protected:
	virtual std::shared_ptr<Message> PollQueue() override;

	virtual void ProcessMessage(std::shared_ptr<Message> message) override;


private:
	MeshLoadingSystem();
	virtual ~MeshLoadingSystem() override {};

	void SerialiseMesh(const std::string& outputPath, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);
	void LoadModelFromKMSH(const std::string& filepath, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);

	std::string CheckForBinary(const std::string& filename);

	void LoadObj(const std::string& filename, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);
	void LoadObjToBinary(const std::string& filename);
	
	std::vector<std::string> Triangularise(const std::vector<std::string>& polygon);
	std::vector<int> ParseFacePoint(const std::string& point);


	//Command Message Functions
	void GenerateBinaries();


	//Command Message Helpers
	std::vector<std::string> FindAllModelFilepaths(std::string& rootFolder);
	std::string StripFilenameFromFilepath(std::string filepath);

};