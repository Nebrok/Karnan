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
	_mesh->Bind(commandBuffer);
	_mesh->Draw(commandBuffer);
}

void GameObject::CreateMesh(const std::string& filename)
{
	std::unique_ptr<BasicMesh> mesh = std::unique_ptr<BasicMesh>(DBG_NEW BasicMesh(filename));
	_mesh = move(mesh);
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
