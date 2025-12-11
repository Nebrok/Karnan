#include "SceneDataObject.h"


//cereal serialisation
#include "cereal/archives/xml.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/memory.hpp>

#include <sstream>
#include <fstream>

bool SceneDataObject::SaveScene(const std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
	std::stringstream ss;

	//ss << gameObjects.size() << '\n';
	{
		cereal::XMLOutputArchive oarchive(ss);

		for (auto go : gameObjects)
		{
			oarchive(go);
		}
	}

	std::ofstream outFile;
	outFile.open("assets/scenes/" + SceneName + ".kscn");
	outFile << ss.rdbuf();
	outFile.close();
    
	return true;
}

bool SceneDataObject::LoadScene(std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
	std::stringstream ss;


	std::ifstream inFile("assets/scenes/" + SceneName + ".kscn");
	if (inFile)
	{
		ss << inFile.rdbuf();
	}
	else
	{
		std::cout << "Could not open file: " << "assets/scenes/" + SceneName + ".kscn" << '\n';
	}

	int numberGOs = 7;
	//ss >> numberGOs;
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
