#pragma once
#include "../GameObject.h"
#include <chrono>


class Stopwatch : public GameObject
{
public:
	int StopwatchNumber = 0;
	int Level = 0;

	bool IsDisplay = false;
private:
	GameObject* _minuteHand;
	GameObject* _secondHand;

	std::chrono::high_resolution_clock::time_point _clockStartTime;
	float _totalSeconds = 0;

public:
	Stopwatch();
	~Stopwatch();


	void Init() override;
	void Start() override;
	void Update(double deltaTime) override;


	void UpdateHands();

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<GameObject>(this));
		ar(cereal::make_nvp("Level", Level));
		ar(cereal::make_nvp("IsDisplay", IsDisplay));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<GameObject>(this));
		ar(cereal::make_nvp("Level", Level));
		ar(cereal::make_nvp("IsDisplay", IsDisplay));
	};

private:

};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(Stopwatch);
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObject, Stopwatch);