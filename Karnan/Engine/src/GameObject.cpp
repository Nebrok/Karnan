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
		component->SetGameobject(this);
		component->Init();
	}
}

void GameObject::Start()
{
}

void GameObject::Update(double deltaTime)
{
	if (!_startCalled && EngineCore::Instance->PlayMode())
	{
		Start();
		_startCalled = true;
	}

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
	if (_material == nullptr || _materialChanged)
	{
		_material = AssetManager::Instance->GetMaterial(_materialName);
		_materialChanged = false;
	}

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

void GameObject::UpdateMaterial()
{
	_material = AssetManager::Instance->GetMaterial(_materialName);
	if (_material != nullptr)
	{
		_renderable = true;
	}
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

void GameObject::RemoveComponent(int index)
{
	std::shared_ptr<ScriptableComponent> componentRemoved;
	componentRemoved = _components[index];
	_components.erase(_components.begin() + index);
}
