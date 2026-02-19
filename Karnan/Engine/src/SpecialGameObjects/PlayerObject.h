#pragma once
#include "../GameObject.h"
#include "../KarnanCamera.h"

class PlayerObject : public GameObject
{
public:


private:
	KarnanCamera* _camera = nullptr;
	glm::vec3 _cameraOffset = { 0.0f, 2.0f, -2.0f };


public:
	PlayerObject();
	~PlayerObject();


	void Init() override;
	void Start() override;
	void Update(double deltaTime) override;

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<GameObject>(this));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<GameObject>(this));
	};

private:

};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(PlayerObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObject, PlayerObject);