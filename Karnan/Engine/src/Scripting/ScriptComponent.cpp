#include "ScriptComponent.h"

#include "RotateGameObject.h"
#include "PlayerController.h"
#include "Physics.h"
#include "CameraFollowPlayer.h"
#include "EndGoal.h"
#include "Key.h"
#include "KeySystem.h"
#include "Door.h"


std::map<std::string, ScriptableComponent* (*)()> ScriptRegister::TypeMap = {};

void ScriptRegister::RegisterTypes()
{
	TypeMap["RotateGameObject"] = &CreateInstance<RotateGameObject>;
	TypeMap["PlayerController"] = &CreateInstance<PlayerController>;
	TypeMap["Physics"] = &CreateInstance<Physics>;
	TypeMap["CameraFollow"] = &CreateInstance<CameraFollow>;
	TypeMap["EndGoal"] = &CreateInstance<EndGoal>;
	TypeMap["Key"] = &CreateInstance<Key>;
	TypeMap["KeySystem"] = &CreateInstance<KeySystem>;
	TypeMap["Door"] = &CreateInstance<Door>;
}

std::vector<std::string> ScriptRegister::GetScriptNames()
{
	std::vector<std::string> keys;
	
	for (auto it = TypeMap.begin(); it != TypeMap.end(); ++it) {
		std::string key = it->first;
		keys.push_back(key);
	}
	return keys;
}

