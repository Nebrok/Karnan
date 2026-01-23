#pragma once
#include "../MessagingSystem/MessageSystem.h"
#include "../BasicMesh.h"
#include "../KarnanMaterial.h"
#include "MaterialDataObject.h"


//std libs
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class GameObject;

class AssetManager : public IMessageSystem
{

public:
	static AssetManager* Instance;

private:
	std::unordered_map<std::string, std::shared_ptr<BasicMesh>> _meshMap;

	std::unordered_map<std::string, std::shared_ptr<KarnanTexture>> _textureMap;
	std::unordered_map<std::string, std::shared_ptr<KarnanMaterial>> _materialMap;
	std::unordered_map<std::string, std::shared_ptr<MaterialDataObject>> _materialDataMap;

	std::unordered_map<std::string, std::vector<GameObject*>> _loadRequested;


	KarnanDevice& _karnanDevice;

public:
	static AssetManager* StartupAssetManager();
	static void DestroyAssetManager();

	void Process();

	std::shared_ptr<BasicMesh> GetMesh(const std::string& filename);
	std::vector<std::string> GetLoadedMeshes();
	std::vector<std::string> FindMeshBinariesInAssetFolder();

	void LoadMaterialDataFromDisk();
	bool AddNewMaterialDataObject();
	bool UpdateMaterialDataMap(std::string oldFilepathKey);
	bool IsMaterialDataLoaded(std::string filepath) 
	{ 
		return _materialDataMap.contains(filepath); 
	};
	std::shared_ptr<MaterialDataObject> GetMaterialData(const std::string& filepath);
	std::vector<std::string> FindMaterialPathsInAssetFolder();


	std::vector<std::string> FindTexturePathsInAssetFolder();




	std::shared_ptr<KarnanMaterial> GetMaterial(const std::string& filename);

	bool IsTextureLoaded(std::string filepath) { return _textureMap.contains(filepath); };
	bool CreateTexture(std::string filepath);
	std::shared_ptr<KarnanTexture> GetTexture(const std::string& filepath);


	virtual void QueueMessage(std::shared_ptr<Message> message) override;

protected:
	// ------------ Message System -----------------------------------------
	virtual void ProcessMessage(std::shared_ptr<Message> message) override;

	//Returns top message in queue and removes message from queue
	virtual std::shared_ptr<Message> PollQueue() override;

private:
	AssetManager();
	virtual ~AssetManager() override {}


	void CreateMesh(const std::string& modelName, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);

};