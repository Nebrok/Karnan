#include "Enemy.h"

#include "../GameObject.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	ScriptableComponent::Init();
	_componentName = "Enemy";
}

void Enemy::Start()
{
	ScriptableComponent::Start();
	_startingPoint = _gameobject->Transform.Translation;

	_patrolStart = _startingPoint - _patrolExtent;
	_patrolEnd = _startingPoint + _patrolExtent;
	_patrolStartToEnd = _patrolEnd - _patrolStart;

}

void Enemy::Update(float deltaTime)
{
	ScriptableComponent::Update(deltaTime);

	_patrolProgress += _patrolSpeed * deltaTime * _patrolDir;
	if (_patrolProgress <= 0.0f)
		_patrolDir *= -1.0f;
	if (_patrolProgress >= 1.0f)
		_patrolDir *= -1.0f;

	_gameobject->Transform.Translation = _patrolStart + _patrolStartToEnd * _patrolProgress;
}

void Enemy::ImGuiRender()
{
	ImGui::DragFloat3("Patrol Extent", glm::value_ptr(_patrolExtent), 0.01f);
	ImGui::DragFloat("Patrol Speed", &_patrolSpeed, 0.01f);
}

CEREAL_REGISTER_DYNAMIC_INIT(Enemy)