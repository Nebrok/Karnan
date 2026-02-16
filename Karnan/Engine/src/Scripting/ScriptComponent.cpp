#include "ScriptComponent.h"

#include "RotateGameObject.h"

std::map<std::string, ScriptableComponent* (*)()> ScriptRegister::TypeMap = {};

void ScriptRegister::RegisterTypes()
{
	TypeMap["RotateGameObject"] = &CreateInstance<RotateGameObject>;
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

