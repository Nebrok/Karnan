#include "GameObject.h"

#include "EngineCore.h"
#include "MessagingSystem/Messages.h"
#include "AssetManagement/AssetManager.h"

#include <iostream>

GameObject::GameObject(const char* objectName)
	: _karnanDevice(EngineCore::Device()), ObjectName(objectName)
{
	_objectId = GenerateNewId();
}

GameObject::~GameObject()
{

}

bool GameObject::HasTag(std::string compareTag)
{
	for (auto tag : Tags)
	{
		if (tag.compare(compareTag) == 0)
		{
			return true;
		}
	}
	return false;
}

void GameObject::Init()
{

}

void GameObject::Update(double deltaTime)
{
	if (_meshRefreshed)
	{
		_meshRefreshed = false;
	}
	if (_material == nullptr)
		_material = AssetManager::Instance->GetMaterial(_materialName);

	if (_material != nullptr)
	{
		_renderable = true;
	}
}

void GameObject::Render(VkCommandBuffer commandBuffer)
{
	std::shared_ptr<BasicMesh> mesh = AssetManager::Instance->GetMesh(_meshName);
	if (mesh == nullptr)
		return;

	mesh->Bind(commandBuffer);
	mesh->Draw(commandBuffer);
}

void GameObject::CreateMesh(const std::string& filename)
{
	std::shared_ptr<AMLoadMeshMessage> message = std::shared_ptr<AMLoadMeshMessage>(DBG_NEW AMLoadMeshMessage(Message::System::NONE, filename, this));
	std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
	AssetManager::Instance->QueueMessage(message);
	messageQueueLock.unlock();
	_meshName = filename;
}

void GameObject::CreateMaterial(const std::string& filename)
{
	MaterialConstructParams constructParams;
	for (auto& textureFilepath : constructParams.Textures)
	{
		textureFilepath = "";
	}
	constructParams.Textures[0] = filename;
	constructParams.MaterialName = filename;

	std::shared_ptr<AMCreateMaterialMessage> message = std::shared_ptr<AMCreateMaterialMessage>(DBG_NEW AMCreateMaterialMessage(Message::System::NONE, constructParams, this));
	std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
	AssetManager::Instance->QueueMessage(message);
	messageQueueLock.unlock();
	_materialName = filename;
}

void GameObject::AddMaterial(const std::string& filename)
{
	std::shared_ptr<KarnanMaterial> material = AssetManager::Instance->GetMaterial(filename);
	if (material == nullptr)
	{
		_materialName = "NONE";
	}
	_material = material;
}
