#pragma once

#include "IPanel.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std libs
#include <string>

struct NewGameObjectData
{
	std::string ObjectName;
	glm::vec3 Translation;
	glm::vec3 Rotation;
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
};

class EditorGOCreatorPanel : public IPanel
{

public:


private:


public:
	virtual void OnImGUIRender() override;

private:
	void CreateNewGameObject(NewGameObjectData data);


};