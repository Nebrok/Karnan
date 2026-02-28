#include "PersistentStore.h"

#include <sstream>
#include <fstream>

PersistentStore* PersistentStore::Instance = nullptr;

PersistentStore::PersistentStore()
{

}

PersistentStore::~PersistentStore()
{
}

void PersistentStore::SaveStore()
{
	std::stringstream ss;

	cereal::XMLOutputArchive oarchive(ss);

	oarchive(std::shared_ptr<PersistentStore>(Instance));

	std::ofstream outFile;
	outFile.open("./defaults/PersistentStore.pst");
	outFile << ss.rdbuf();
	outFile.close();

	return;
}

void PersistentStore::LoadStore()
{
	std::stringstream ss;

	std::ifstream inFile("./defaults/PersistentStore.pst");
	if (inFile)
	{
		ss << inFile.rdbuf();
	}
	else
	{
		std::cout << "Could not open store: " << '\n';
	}

	cereal::XMLInputArchive ar(ss);

	ar(std::shared_ptr<PersistentStore>(Instance));


	return;
}
