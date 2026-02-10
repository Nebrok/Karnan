#pragma once

#include "../GameObject.h"

#include <string>
#include <filesystem>
#include <vector>
#include <memory>

class SceneDataObject
{
	
public:
	std::string SceneName;
	std::filesystem::path filePath;

	bool SaveScene(const std::vector<std::shared_ptr<GameObject>>& gameObjects);
	bool LoadScene(std::vector<std::shared_ptr<GameObject>>& gameObjects);

	void CreateNewSceneFromDefault(std::vector<std::shared_ptr<GameObject>>& gameObjects);


};