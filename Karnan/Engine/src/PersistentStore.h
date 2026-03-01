#pragma once

#include <string>

//cereal
#include <cereal/types/memory.hpp>
#include <cereal/archives/xml.hpp>

class PersistentStore
{
public:
    float LevelOneTime = 100000.0f;
    float LevelTwoTime = 100000.0f;
    float LevelThreeTime = 100000.0f;

private:


public:
    void SaveStore();
    void LoadStore();

    //Cereal serialisation
    template <class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::make_nvp("LevelOneTime", LevelOneTime));
        ar(cereal::make_nvp("LevelTwoTime", LevelTwoTime));
        ar(cereal::make_nvp("LevelThreeTime", LevelThreeTime));
    };

    template <class Archive>
    void load(Archive& ar)
    {
        ar(cereal::make_nvp("LevelOneTime", LevelOneTime));
        ar(cereal::make_nvp("LevelTwoTime", LevelTwoTime));
        ar(cereal::make_nvp("LevelThreeTime", LevelThreeTime));
    };

    template <class Archive>
    static void load_and_construct(Archive& ar, cereal::construct<PersistentStore>& construct)
    {
        construct();

        float levelOne;
        ar(cereal::make_nvp("LevelOneTime", levelOne));
        construct->LevelOneTime = levelOne;

        float levelTwo;
        ar(cereal::make_nvp("LevelTwoTime", levelTwo));
        construct->LevelTwoTime = levelTwo;

        float levelThree;
        ar(cereal::make_nvp("LevelThreeTime", levelThree));
        construct->LevelThreeTime = levelThree;
    }

private:

};