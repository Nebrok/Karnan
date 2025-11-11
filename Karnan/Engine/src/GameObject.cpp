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
