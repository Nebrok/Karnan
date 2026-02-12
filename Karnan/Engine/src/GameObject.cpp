#include "GameObject.h"

#include <iostream>

#include "EngineCore.h"
#include "MessagingSystem/Messages.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Colliders.h"

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
	if (_collider == nullptr)
	{
		_collider = std::shared_ptr<SphereCollider>(DBG_NEW SphereCollider());
		_colliderActive = false;
	}
	_collider->GameObject = this;


	for (auto component : _components)
	{
		component->Init();
	}
}

void GameObject::Update(double deltaTime)
{
	if (_meshRefreshed)
	{
		_meshRefreshed = false;
	}

	if (_material == nullptr || _materialChanged)
	{
		_material = AssetManager::Instance->GetMaterial(_materialName);
		_materialChanged = false;
	}

	if (_material != nullptr)
	{
		_renderable = true;
	}
	
	for (auto component : _components)
	{
		component->Update(deltaTime);
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
	std::shared_ptr<AMCreateMaterialMessage> message = std::shared_ptr<AMCreateMaterialMessage>(DBG_NEW AMCreateMaterialMessage(Message::System::NONE, filename, this));
	std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
	AssetManager::Instance->QueueMessage(message);
	messageQueueLock.unlock();
	_materialName = filename;
	_materialChanged = true;
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
