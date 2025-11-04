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

void GameObject::Update(float deltaTime)
{
	Transform.Rotation.y += 1 * deltaTime;
}

void GameObject::Render(VkCommandBuffer commandBuffer)
{
	_mesh->Bind(commandBuffer);
	_mesh->Draw(commandBuffer);
}
