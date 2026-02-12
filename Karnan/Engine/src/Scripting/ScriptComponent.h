#pragma once

#include <string>
#include <map>

class GameObject;

class ScriptableComponent
{
public:

protected:
	GameObject* _gameobject = nullptr;
	std::string _componentName = "Blank";


public:
	ScriptableComponent() {};
	virtual ~ScriptableComponent() {};

	virtual void Init() {};
	virtual void Update(float deltaTime) {};
	virtual void PhysicsUpdate(float deltaTime) {};

	std::string GetName() { return _componentName; };

	void SetGameobject(GameObject* go) { _gameobject = go; };

private:



};




class ScriptRegister
{
public:
	template<typename T> 
	static ScriptableComponent* CreateInstance() { return new T; }
	
	
	static std::map<std::string, ScriptableComponent* (*)()> TypeMap;
	static void RegisterTypes();

};


