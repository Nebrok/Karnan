#include "KeySystem.h"

KeySystem::KeySystem()
{
}

KeySystem::~KeySystem()
{
}

void KeySystem::Init()
{
	ScriptableComponent::Init();
	_componentName = "Key System";
}

void KeySystem::Start()
{
	ScriptableComponent::Start();
}

void KeySystem::Update(float deltaTime)
{
	ScriptableComponent::Update(deltaTime);
}

void KeySystem::ImGuiRender()
{
}

CEREAL_REGISTER_DYNAMIC_INIT(KeySystem)
