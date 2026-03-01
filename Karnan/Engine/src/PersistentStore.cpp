#include "PersistentStore.h"

#include <sstream>
#include <fstream>

#include "EngineCore.h"

void PersistentStore::SaveStore()
{	
	std::stringstream ss;

	{
	cereal::XMLOutputArchive oarchive(ss);

	oarchive(LevelOneTime);
	oarchive(LevelTwoTime);
	oarchive(LevelThreeTime);
	}


	std::ofstream outFile;
	outFile.open("./defaults/PersistentStore.kpst");
	outFile << ss.rdbuf();
	outFile.close();

	return;	
}

void PersistentStore::LoadStore()
{
	
	std::stringstream ss;

	std::ifstream inFile("./defaults/PersistentStore.kpst");
	if (inFile)
	{
		ss << inFile.rdbuf();
	}
	else
	{
		std::cout << "Could not open store: " << '\n';
		return;
	}
	inFile.close();
	cereal::XMLInputArchive ar(ss);

	ar(LevelOneTime);
	ar(LevelTwoTime);
	ar(LevelThreeTime);

	return;
}
