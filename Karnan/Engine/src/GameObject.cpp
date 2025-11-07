#include "GameObject.h"

GameObject::GameObject(KarnanDevice& device)
	: _karnanDevice(device)
{
	
}

GameObject::~GameObject()
{

}

void GameObject::Init()
{

}

void GameObject::Update(float deltaTime, float rotationSpeed)
{
	Transform.Rotation.y += rotationSpeed * deltaTime;
}

void GameObject::Render(VkCommandBuffer commandBuffer)
{
	_mesh->Bind(commandBuffer);
	_mesh->Draw(commandBuffer);
}
