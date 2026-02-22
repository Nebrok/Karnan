#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <memory>

class GameObject;

class SceneDataObject
{
	
public:
	std::string SceneName;
	std::filesystem::path FilePath = "assets/scenes/";

	bool SaveScene(const std::vector<std::shared_ptr<GameObject>>& gameObjects);
	bool LoadScene(std::vector<std::shared_ptr<GameObject>>& gameObjects);

	void CreateNewSceneFromDefault(std::vector<std::shared_ptr<GameObject>>& gameObjects);

	void SetFilePath(std::string filepath);


};