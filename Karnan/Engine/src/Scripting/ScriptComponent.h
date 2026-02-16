#pragma once

#include <string>
#include <map>
#include <vector>

//cereal
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/xml.hpp>
//#include "../Serialisation/DefinitionsKarnanCereal.h"

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


	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::make_nvp("ComponentName", _componentName));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::make_nvp("ComponentName", _componentName));
	};

	template <class Archive>
	static void load_and_construct(Archive& ar, cereal::construct<ScriptableComponent>& construct)
	{
		std::string componentName;
		ar(cereal::make_nvp("ComponentName", componentName));
		construct->_componentName = componentName;
	}

private:



};




class ScriptRegister
{
public:
	template<typename T> 
	static ScriptableComponent* CreateInstance() { return new T; }
	
	
	static std::map<std::string, ScriptableComponent* (*)()> TypeMap;
	static void RegisterTypes();
	static std::vector<std::string> GetScriptNames();

};


