#include "AssetManager.h"

#include "../EngineCore.h"
#include "../MessagingSystem/Messages.h"

#include <iostream>
#include <filesystem>


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

std::vector<std::string> AssetManager::GetLoadedMeshes()
{
	std::vector<std::string> meshNames;
	for (auto keyValue : _meshMap)
	{
		meshNames.push_back(keyValue.first);
	}
	return meshNames;
}

std::vector<std::string> AssetManager::FindMeshBinariesInAssetFolder()
{
	std::vector<std::string> objFiles;

	for (const auto& entry : std::filesystem::recursive_directory_iterator("./assets")) {
		if (entry.is_regular_file() && (entry.path().extension() == ".obj" || entry.path().extension() == ".OBJ")) {
			// Store the path relative to the root folder
			objFiles.push_back(
				std::filesystem::relative(entry.path(), std::filesystem::current_path()).generic_string()
			);
		}
	}

	return objFiles;
}

void AssetManager::LoadMaterialDataFromDisk()
{
	std::vector<std::string> filepaths;

	for (const auto& entry : std::filesystem::recursive_directory_iterator("./assets")) {
		if (entry.is_regular_file() && (entry.path().extension() == ".kmat")) {
			// Store the path relative to the root folder
			filepaths.push_back(std::filesystem::relative(entry.path(), std::filesystem::current_path()).generic_string());
		}
	}

	for (std::string filepath : filepaths)
	{
		std::shared_ptr<MaterialDataObject> newDataObject = std::shared_ptr<MaterialDataObject>(DBG_NEW MaterialDataObject());
		newDataObject->LoadMaterial(filepath);
		_materialDataMap[filepath] = newDataObject;
	}

}

bool AssetManager::AddNewMaterialDataObject()
{
	std::shared_ptr<MaterialDataObject> newDataObject = std::shared_ptr<MaterialDataObject>(DBG_NEW MaterialDataObject());
	newDataObject->MaterialName = "New Material";
	std::string newFilepath = newDataObject->SaveMaterial();
	_materialDataMap[newFilepath] = newDataObject;
	newDataObject->Filepath = newFilepath;
	return true;
}

bool AssetManager::UpdateMaterialDataMap(std::string oldFilepathKey)
{
	std::string newKey = _materialDataMap[oldFilepathKey]->Filepath;
	_materialDataMap[newKey] = _materialDataMap[oldFilepathKey];
	_materialDataMap.erase(oldFilepathKey);
	
	return true;
}

std::shared_ptr<MaterialDataObject> AssetManager::GetMaterialData(const std::string& filepath)
{
	if (!_materialDataMap.contains(filepath))
		return nullptr;

	return _materialDataMap.at(filepath);
}

std::vector<std::string> AssetManager::FindMaterialPathsInAssetFolder()
{
	std::vector<std::string> materialFiles;

	for (const auto& entry : std::filesystem::recursive_directory_iterator("assets")) {
		if (entry.is_regular_file() && (entry.path().extension() == ".kmat")) {
			// Store the path relative to the root folder
			materialFiles.push_back(
				std::filesystem::relative(entry.path(), std::filesystem::current_path()).generic_string()
			);
		}
	}

	return materialFiles;
}

std::vector<std::string> AssetManager::FindTexturePathsInAssetFolder()
{
	std::vector<std::string> textureFiles;

	for (const auto& entry : std::filesystem::recursive_directory_iterator("assets")) {
		if (entry.is_regular_file() && (entry.path().extension() == ".png")) {
			// Store the path relative to the root folder
			textureFiles.push_back(
				std::filesystem::relative(entry.path(), std::filesystem::current_path()).generic_string()
			);
		}
	}

	return textureFiles;
}

std::shared_ptr<KarnanMaterial> AssetManager::GetMaterial(const std::string& filename)
{
	if (auto search = _materialMap.find(filename); search != _materialMap.end())
	{
		return _materialMap.at(filename);
	}
	return nullptr;
}

bool AssetManager::CreateTexture(std::string filepath)
{
	std::shared_ptr<KarnanTexture> newTexture = std::shared_ptr<KarnanTexture>(DBG_NEW KarnanTexture(filepath));
	if (newTexture == nullptr)
		return false;

	_textureMap[filepath] = newTexture;
	return true;
}

std::shared_ptr<KarnanTexture> AssetManager::GetTexture(const std::string& filepath)
{
	if (!_textureMap.contains(filepath))
		return nullptr;

	return _textureMap.at(filepath);
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
		if ((message->MessageInfo()).compare("Update Gameobject Material") == 0)
		{
			AMUpdateGameobjectMaterialMessage* relevantMessage = dynamic_cast<AMUpdateGameobjectMaterialMessage*>(message.get());
			relevantMessage->CallingGO->UpdateMaterial();
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
			AMCreateMaterialMessage* relevantMessage = dynamic_cast<AMCreateMaterialMessage*>(message.get());
			std::string materialDataObjectFilepath = dynamic_cast<AMCreateMaterialMessage*>(message.get())->MaterialDataObjectFilepath;

			if (auto search = _loadRequested.find(materialDataObjectFilepath); search != _loadRequested.end())
			{
				std::cout << "Material: " << materialDataObjectFilepath << " has already been requested to load." << '\n';
				
				std::shared_ptr<AMUpdateGameobjectMaterialMessage> updateMessage = std::shared_ptr<AMUpdateGameobjectMaterialMessage>(
					DBG_NEW AMUpdateGameobjectMaterialMessage(Message::System::ASSET_MANAGER, relevantMessage->CallingGO));

				std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
				AssetManager::Instance->QueueMessage(updateMessage);
				messageQueueLock.unlock();

				return;
			}


			std::shared_ptr<MaterialDataObject> matData = _materialDataMap[materialDataObjectFilepath];
			std::shared_ptr<KarnanMaterial> material = std::shared_ptr<KarnanMaterial>(DBG_NEW KarnanMaterial());
			_materialMap[materialDataObjectFilepath] = material;
			material->SetMinFilter(matData->MinFilter);
			material->SetMagFilter(matData->MagFilter);
			material->SetAddressMode(matData->SamplerAddressMode);

			material->Init();
			int index = 0;
			for (auto textureFilepath : matData->Textures)
			{
				if (textureFilepath == "")
					continue;
				material->CreateTextureInSlot(index, textureFilepath);
				index++;
			}
			material->CreateImageInfos();

			std::shared_ptr<AMUpdateGameobjectMaterialMessage> updateMessage = std::shared_ptr<AMUpdateGameobjectMaterialMessage>(
				DBG_NEW AMUpdateGameobjectMaterialMessage(Message::System::ASSET_MANAGER, relevantMessage->CallingGO));

			std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
			AssetManager::Instance->QueueMessage(updateMessage);
			messageQueueLock.unlock();

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
