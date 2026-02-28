#pragma once

#include <unordered_map>
#include <string>


//cereal
#include <cereal/types/memory.hpp>
#include <cereal/archives/xml.hpp>
#include "Serialisation/DefinitionsKarnanCereal.h"

class PersistentStore
{
public:
	static PersistentStore* Instance;

private:
	static std::unordered_map<std::string, float> _floatMap;

public:
	PersistentStore();
	~PersistentStore();

    static void SaveStore();
    static void LoadStore();

    //Cereal serialisation
    template <class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::make_nvp("Float Map", _floatMap));
    };

    template <class Archive>
    void load(Archive& ar)
    {
        ar(cereal::make_nvp("Float Map", _floatMap));
    };

private:

};