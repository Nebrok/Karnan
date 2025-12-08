#pragma once
#include "../MessagingSystem/MessageSystem.h"
#include "../BasicMesh.h"
#include "../KarnanMaterial.h"

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

	std::unordered_map<std::string, std::shared_ptr<KarnanMaterial>> _materialMap;


	std::unordered_map<std::string, std::vector<GameObject*>> _loadRequested;


	KarnanDevice& _karnanDevice;

public:
	static AssetManager* StartupAssetManager();
	static void DestroyAssetManager();

	void Process();

	std::shared_ptr<BasicMesh> GetMesh(const std::string& filename);
	std::shared_ptr<KarnanMaterial> GetMaterial(const std::string& filename);

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