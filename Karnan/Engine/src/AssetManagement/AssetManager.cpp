#include "AssetManager.h"

#include "../EngineCore.h"
#include "../MessagingSystem/Messages.h"
#include "../KarnanMaterial.h"

#include <iostream>


AssetManager* AssetManager::Instance = nullptr;

AssetManager* AssetManager::StartupAssetManager()
{
	if (AssetManager::Instance == nullptr)
	{
		Instance = DBG_NEW AssetManager();
	}
	return Instance;
}

void AssetManager::DestroyAssetManager()
{
	delete(AssetManager::Instance);
}

void AssetManager::Process()
{
	std::unique_lock<std::mutex> messageQueueLock(MessageQueueMutex);
	std::shared_ptr<Message> message = PollQueue();
	messageQueueLock.unlock();
	while (message != nullptr)
	{
		ProcessMessage(message);
		std::unique_lock<std::mutex> messageQueueLock(MessageQueueMutex);
		message = PollQueue();
		messageQueueLock.unlock();
	}
}

std::shared_ptr<BasicMesh> AssetManager::GetMesh(const std::string& filename)
{
	if (auto search = _meshMap.find(filename); search != _meshMap.end())
	{
		return _meshMap.at(filename);
	}
	return nullptr;
}

std::shared_ptr<KarnanMaterial> AssetManager::GetMaterial(const std::string& filename)
{
	if (auto search = _materialMap.find(filename); search != _materialMap.end())
	{
		return _materialMap.at(filename);
	}
	return nullptr;
}

void AssetManager::QueueMessage(std::shared_ptr<Message> message)
{
	_messages.push(message);
}

void AssetManager::ProcessMessage(std::shared_ptr<Message> message)
{
	if (message->GetMessageType() == Message::Type::REPLY)
	{
		if ((message->MessageInfo()).compare("Load Mesh Reply") == 0)
		{
			Message* key = dynamic_cast<AMReplyLoadMeshMessage*>(message.get())->OriginalMessageKey;
			std::string filepath = "";
			if (_awaitingReply.contains(key))
			{
				std::shared_ptr<Message> awaitingReplyMessage = _awaitingReply.at(key);
				GameObject* go = dynamic_cast<AMLoadMeshMessage*>(awaitingReplyMessage.get())->CallingGO;
				go->SetMeshRefreshed();
				filepath = dynamic_cast<AMLoadMeshMessage*>(awaitingReplyMessage.get())->Filepath;
			}
			_awaitingReply.erase(key);
			if (auto search = _loadRequested.find(filepath); search != _loadRequested.end())
			{
				for (auto go : _loadRequested[filepath])
				{
					go->SetMeshRefreshed();
				}
				_loadRequested.erase(filepath);
			}
		}
	}
	
	if (message->GetMessageType() == Message::Type::COMMAND)
	{
		if ((message->MessageInfo()).compare("Create Mesh") == 0)
		{
			AMCreateMeshMessage* createMeshMessage = dynamic_cast<AMCreateMeshMessage*>(message.get());
			CreateMesh(createMeshMessage->Meshname, *(createMeshMessage->Vertices), *(createMeshMessage->Indices));
			delete(createMeshMessage->Vertices);
			delete(createMeshMessage->Indices);
		}
		if ((message->MessageInfo()).compare("Load Mesh") == 0)
		{
			std::string filepath = dynamic_cast<AMLoadMeshMessage*>(message.get())->Filepath;

			if (auto search = _meshMap.find(filepath); search != _meshMap.end())
			{
				std::cout << "Model: " << filepath << " is already loaded." << '\n';
				return;
			}
			if (auto search = _loadRequested.find(filepath); search != _loadRequested.end())
			{
				std::cout << "Model: " << filepath << " has already been requested to load." << '\n';
				_loadRequested[filepath].push_back(dynamic_cast<AMLoadMeshMessage*>(message.get())->CallingGO);
				return;
			}

			std::shared_ptr<MLSLoadModelMessage> loadMessage = std::shared_ptr<MLSLoadModelMessage>(DBG_NEW MLSLoadModelMessage(Message::System::ASSET_MANAGER, filepath));
			loadMessage->ReplyKey = message.get();
			std::unique_lock<std::mutex> messageQueueLock(MeshLoadingSystem::Instance->MessageQueueMutex);
			MeshLoadingSystem::Instance->QueueMessage(loadMessage);
			messageQueueLock.unlock();
			_awaitingReply[message.get()] = message;
			_loadRequested[filepath].push_back(dynamic_cast<AMLoadMeshMessage*>(message.get())->CallingGO);
		}
		if ((message->MessageInfo()).compare("Create Material") == 0)
		{
			MaterialConstructParams& materialConstructionDetails = dynamic_cast<AMCreateMaterialMessage*>(message.get())->MaterialConstructInfo;
			std::string filepath = materialConstructionDetails.MaterialName;

			if (auto search = _loadRequested.find(filepath); search != _loadRequested.end())
			{
				std::cout << "Material: " << filepath << " has already been requested to load." << '\n';
				_loadRequested[filepath].push_back(dynamic_cast<AMCreateMaterialMessage*>(message.get())->CallingGO);
				return;
			}

			std::shared_ptr<KarnanMaterial> material = std::shared_ptr<KarnanMaterial>(DBG_NEW KarnanMaterial());
			_materialMap[filepath] = material;
			material->Init();
			int index = 0;
			for (auto textureFilepath : materialConstructionDetails.Textures)
			{
				if (textureFilepath == "")
					continue;
				material->CreateTextureInSlot(index, textureFilepath);
				index++;
			}
			material->CreateImageInfos();

			_loadRequested[filepath].push_back(dynamic_cast<AMCreateMaterialMessage*>(message.get())->CallingGO);

		}
	}
}

std::shared_ptr<Message> AssetManager::PollQueue()
{
	if (_messages.empty())
		return nullptr;

	std::shared_ptr<Message> message = _messages.front();
	_messages.pop();
	return message;
}

AssetManager::AssetManager()
	: _karnanDevice(EngineCore::Device())
{

}

void AssetManager::CreateMesh(const std::string& modelName, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(_karnanDevice));
	vertexBuffer->CreateVertexBuffers(vertices);

	std::shared_ptr<BasicMesh> mesh;
	if (indices.size() > 0)
	{
		std::shared_ptr<IndexBuffer> indexBuffer(DBG_NEW IndexBuffer(_karnanDevice));
		indexBuffer->CreateIndexBuffers(indices);
		mesh = std::shared_ptr<BasicMesh>(DBG_NEW BasicMesh(modelName, vertexBuffer, indexBuffer));
	}
	else
	{
		mesh = std::shared_ptr<BasicMesh>(DBG_NEW BasicMesh(modelName, vertexBuffer, nullptr));
	}

	_meshMap.insert({ modelName, mesh });
}
