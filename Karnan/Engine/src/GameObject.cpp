#include "GameObject.h"

#include "EngineCore.h"
#include "MessagingSystem/Messages.h"
#include "AssetManagement/AssetManager.h"

GameObject::GameObject(const char* objectName)
	: _karnanDevice(EngineCore::Device()), ObjectName(objectName)
{
	_objectId = GenerateNewId();
	EngineCore::AddGameObjectToActiveScene(this);
}

GameObject::~GameObject()
{

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
	std::unique_ptr<KarnanMaterial> material = std::unique_ptr<KarnanMaterial>(DBG_NEW KarnanMaterial());
	_material = move(material);
	_material->Init();
	_material->CreateTextureInSlot(0, filename.c_str());
	_material->CreateImageInfos();
	_renderable = true;
}
