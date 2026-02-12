#include "ScriptComponent.h"

#include "RotateGameObject.h"

std::map<std::string, ScriptableComponent* (*)()> ScriptRegister::TypeMap = {};

void ScriptRegister::RegisterTypes()
{
	TypeMap["RotateGameObject"] = &CreateInstance<RotateGameObject>;
}

