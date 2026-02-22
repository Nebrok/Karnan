#include "SceneDataObject.h"


//cereal serialisation
#include "cereal/archives/xml.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/memory.hpp>

#include <sstream>
#include <fstream>
#include "../GameObject.h"

bool SceneDataObject::SaveScene(const std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
	std::stringstream ss;

	ss << gameObjects.size() << '\n';
	{
		cereal::XMLOutputArchive oarchive(ss);

		for (auto go : gameObjects)
		{
			oarchive(go);
		}
	}

	std::ofstream outFile;
	outFile.open(FilePath.string() + SceneName + ".kscn");
	outFile << ss.rdbuf();
	outFile.close();
    
	return true;
}

bool SceneDataObject::LoadScene(std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
	std::stringstream ss;

	std::string inFilepath = FilePath.string() + SceneName + ".kscn";
	std::ifstream inFile(inFilepath);
	if (inFile)
	{
		ss << inFile.rdbuf();
	}
	else
	{
		std::cout << "Could not open file: " << FilePath.string() + SceneName + ".kscn" << '\n';
		std::cout << "Creating new scene with name " << SceneName << "." << '\n';
		CreateNewSceneFromDefault(gameObjects);
		return false;
	}

	int numberGOs;
	ss >> numberGOs;
	{
		cereal::XMLInputArchive ar(ss);

		for (int i = 0; i < numberGOs; i++)
		{
			std::shared_ptr<GameObject> readGameObject;
			ar(readGameObject);
			readGameObject->Init();
			readGameObject->CreateMesh(readGameObject->GetMeshName());
			readGameObject->CreateMaterial(readGameObject->GetMaterialName());
			gameObjects.push_back(readGameObject);
		}
	}

	std::cout << "Number gameObjects in loaded scene: " << numberGOs << '\n';

    return false;
}

void SceneDataObject::CreateNewSceneFromDefault(std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
	std::stringstream ss;


	std::ifstream inFile("defaults/scenes/BlankScene.kscn");
	if (inFile)
	{
		ss << inFile.rdbuf();
	}
	else
	{
		std::cout << "Could not open file: " << FilePath.string() + SceneName + ".kscn" << '\n';
		return;
	}


	int numberGOs;
	ss >> numberGOs;
	{
		cereal::XMLInputArchive ar(ss);

		for (int i = 0; i < numberGOs; i++)
		{
			std::shared_ptr<GameObject> readGameObject;
			ar(readGameObject);
			readGameObject->Init();
			readGameObject->CreateMesh(readGameObject->GetMeshName());
			readGameObject->CreateMaterial(readGameObject->GetMaterialName());
			gameObjects.push_back(readGameObject);
		}
	}

	std::cout << "Number gameObjects in loaded scene: " << numberGOs << '\n';

	return;
}

void SceneDataObject::SetFilePath(std::string filepath)
{
	FilePath = filepath;
}
