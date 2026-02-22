#include "PlayerObject.h"

#include "../EngineCore.h"

PlayerObject::PlayerObject()
	: GameObject("Player")
{
}

PlayerObject::~PlayerObject()
{
}

void PlayerObject::Init()
{
	GameObject::Init();
	Tags.push_back("Player");
}

void PlayerObject::Start()
{
	GameObject::Start();
}

void PlayerObject::Update(double deltaTime)
{
	GameObject::Update(deltaTime);

}