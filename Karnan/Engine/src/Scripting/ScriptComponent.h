#pragma once

#include <string>
#include <map>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "imgui_stdlib.h"
#include "glm/gtc/type_ptr.hpp"

//cereal
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include "../Serialisation/DefinitionsKarnanCereal.h"
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
	virtual void Start() {};
	virtual void Update(float deltaTime) {};
	virtual void PhysicsUpdate(float deltaTime) {};
	virtual void ImGuiRender() {};


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


