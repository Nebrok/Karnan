#include "GameObject.h"

#include "EngineCore.h"

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
	
}

void GameObject::Render(VkCommandBuffer commandBuffer)
{
	MeshLoadingSystem::Instance->GetMesh(_meshName)->Bind(commandBuffer);
	MeshLoadingSystem::Instance->GetMesh(_meshName)->Draw(commandBuffer);
}

void GameObject::CreateMesh(const std::string& filename)
{
	MeshLoadingSystem::Instance->LoadMesh(filename);
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
