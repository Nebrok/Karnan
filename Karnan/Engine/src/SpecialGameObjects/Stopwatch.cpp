#include "Stopwatch.h"

#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"

#include "../EngineCore.h"
#include "../PersistentStore.h"



Stopwatch::Stopwatch()
	: GameObject("Stopwatch")
{
}

Stopwatch::~Stopwatch()
{
	if (IsDisplay)
		return;

	auto currentTime = std::chrono::high_resolution_clock::now();
	float numSeconds = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - _clockStartTime).count();
	_totalSeconds = numSeconds;

	float currentSaved = FLT_MIN;
	switch (Level)
	{
	case 1:
		currentSaved = EngineCore::Instance->GetPersistentStore()->LevelOneTime;
		break;
	case 2:
		currentSaved = EngineCore::Instance->GetPersistentStore()->LevelTwoTime;
		break;
	case 3:
		currentSaved = EngineCore::Instance->GetPersistentStore()->LevelThreeTime;
		break;
	default:
		break;
	}

	if (!(_totalSeconds < currentSaved))
		return;

	switch (Level)
	{
	case 1:
		EngineCore::Instance->GetPersistentStore()->LevelOneTime = _totalSeconds;
		break;
	case 2:
		EngineCore::Instance->GetPersistentStore()->LevelTwoTime = _totalSeconds;
		break;
	case 3:
		EngineCore::Instance->GetPersistentStore()->LevelThreeTime = _totalSeconds;
		break;
	default:
		break;
	}

}

void Stopwatch::Init()
{
	GameObject::Init();
	std::vector<std::shared_ptr<GameObject>> stopwatches = EngineCore::Instance->GetActiveScenePointer()->GetAllGameObjectsWithTag("Stopwatch");
	StopwatchNumber = (int)stopwatches.size();
	Tags.push_back("Stopwatch");

	
}

void Stopwatch::Start()
{
	GameObject::Start();

	std::string minuteHandName = "Minute Hand " + std::to_string(StopwatchNumber);
	std::string secondHandName = "Second Hand " + std::to_string(StopwatchNumber);

	if (std::vector<std::shared_ptr<GameObject>> minuteHands = EngineCore::GetAllGameObjectsInActiveSceneWithName(minuteHandName); minuteHands.size() == 1)
	{
		_minuteHand = minuteHands[0].get();
	}
	else
	{
		GameObject* minuteHand = DBG_NEW GameObject(minuteHandName.c_str());
		minuteHand->Init();
		minuteHand->Transform.Scale = Transform.Scale;
		minuteHand->CreateMesh("./assets/models/HourHand.obj");
		minuteHand->CreateMaterial("assets/materials/ShinyBlack.kmat");
		EngineCore::AddGameObjectToActiveScene(std::shared_ptr<GameObject>(minuteHand));
		_minuteHand = minuteHand;
	}
	_minuteHand->Transform.Translation = Transform.Translation;

	if (std::vector<std::shared_ptr<GameObject>> secondHands = EngineCore::GetAllGameObjectsInActiveSceneWithName(secondHandName); secondHands.size() == 1)
	{
		_secondHand = secondHands[0].get();
	}
	else
	{
		GameObject* secondHand = DBG_NEW GameObject(secondHandName.c_str());
		secondHand->Init();
		secondHand->Transform.Scale = Transform.Scale;
		secondHand->CreateMesh("./assets/models/SecondHand.obj");
		secondHand->CreateMaterial("assets/materials/ShinyBlack.kmat");
		EngineCore::AddGameObjectToActiveScene(std::shared_ptr<GameObject>(secondHand));
		_secondHand = secondHand;
	}
	_secondHand->Transform.Translation = Transform.Translation;

	_clockStartTime = std::chrono::high_resolution_clock::now();

}

void Stopwatch::Update(double deltaTime)
{
	GameObject::Update(deltaTime);
	auto currentTime = std::chrono::high_resolution_clock::now();
	float numSeconds = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - _clockStartTime).count();
	_totalSeconds = numSeconds;


	if (IsDisplay)
	{
		switch (Level)
		{
		case 1:
			_totalSeconds = EngineCore::Instance->GetPersistentStore()->LevelOneTime;
			break;
		case 2:
			_totalSeconds = EngineCore::Instance->GetPersistentStore()->LevelTwoTime;
			break;
		case 3:
			_totalSeconds = EngineCore::Instance->GetPersistentStore()->LevelThreeTime;
			break;
		default:
			break;
		}
	}


	UpdateHands();
}

void Stopwatch::UpdateHands()
{
	float secondHandRotationX = (int)_totalSeconds * 1.0f / 60.0f * 2 * glm::pi<float>();
	float minuteHandRotationX = (int)(_totalSeconds / 60.0f) * 1.0f / 60.0f * 2 * glm::pi<float>();

	glm::quat quatAroundX = glm::angleAxis(Transform.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat quatAroundY = glm::angleAxis(Transform.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat quatAroundZ = glm::angleAxis(Transform.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::quat baseRotation = quatAroundY * quatAroundX * quatAroundZ;

	glm::quat secondHandLocalQuat = glm::angleAxis(secondHandRotationX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat minuteHandLocalQuat = glm::angleAxis(minuteHandRotationX, glm::vec3(1.0f, 0.0f, 0.0f));


	glm::quat secondHandfinalRotation = baseRotation * secondHandLocalQuat;
	glm::quat minuteHandfinalRotation = baseRotation * minuteHandLocalQuat;


	glm::vec3 finalRotationSecondHandEuler;
	glm::extractEulerAngleYXZ(glm::mat4_cast(secondHandfinalRotation), finalRotationSecondHandEuler.y, finalRotationSecondHandEuler.x, finalRotationSecondHandEuler.z);
	_secondHand->Transform.Rotation = finalRotationSecondHandEuler;

	glm::vec3 finalRotationMinuteHandEuler;
	glm::extractEulerAngleYXZ(glm::mat4_cast(minuteHandfinalRotation), finalRotationMinuteHandEuler.y, finalRotationMinuteHandEuler.x, finalRotationMinuteHandEuler.z);
	_minuteHand->Transform.Rotation = finalRotationMinuteHandEuler;
}
