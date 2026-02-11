#pragma once

#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


//cereal
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/xml.hpp>
#include "../Serialisation/DefinitionsKarnanCereal.h"

class GameObject;

class Collider
{
public:
	enum class ColliderType
	{
		NONE,
		BOX,
		SPHERE
	};

	ColliderType Type;
	GameObject* GameObject = nullptr;

	Collider();

	glm::mat4 Transform();
	

	glm::mat4 ScalelessTransform();


	virtual std::string ToString()
	{
		switch (Type)
		{
		case ColliderType::NONE:
			return "NONE";
		case ColliderType::BOX:
			return "BOX";
		case ColliderType::SPHERE:
			return "SPHERE";
		}
	}

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::make_nvp("Type", Type));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		;
		ar(cereal::make_nvp("Type", Type));
	};

	template <class Archive>
	static void load_and_construct(Archive& ar, cereal::construct<Collider>& construct)
	{
		ColliderType type;
		ar(cereal::make_nvp("Type", type));
		construct->Type = type;
	}

private:


};

class SphereCollider : public Collider
{
public:
	float Radius = .5f;

	SphereCollider()
		: Collider()
	{
		Type = ColliderType::SPHERE;
	}

	SphereCollider(float radius)
		: Collider(), Radius(radius)
	{
		Type = ColliderType::SPHERE;
	}

	virtual std::string ToString() override
	{
		switch (Type)
		{
		case ColliderType::NONE:
			return "NONE";
		case ColliderType::BOX:
			return "BOX";
		case ColliderType::SPHERE:
			return "SPHERE";
		}
	}

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<Collider>(this));
		ar(CEREAL_NVP(Radius));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<Collider>(this));
		ar(CEREAL_NVP(Radius));
	};

};

CEREAL_REGISTER_TYPE(SphereCollider);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Collider, SphereCollider);

class BoxCollider : public Collider
{
public:
	glm::vec3 Extent = { 0.5f, 0.5f, 0.5f };

	BoxCollider()
		: Collider()
	{
		Type = ColliderType::BOX;
	}

	BoxCollider(const glm::vec3& extent)
		: Collider(), Extent(extent)
	{
		Type = ColliderType::BOX;
	}

	std::vector<glm::vec3> GetAxises()
	{
		std::vector<glm::vec3> axises = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };

		glm::mat4 rotation = ScalelessTransform();
		rotation[3] -= Transform()[3];
		for (auto& axis : axises)
		{
			axis = rotation * glm::vec4(axis, 1.0f);
		}
		return axises;
	}

	virtual std::string ToString() override
	{
		switch (Type)
		{
		case ColliderType::NONE:
			return "NONE";
		case ColliderType::BOX:
			return "BOX";
		case ColliderType::SPHERE:
			return "SPHERE";
		}
	}

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<Collider>(this));
		ar(CEREAL_NVP(Extent));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<Collider>(this));
		ar(CEREAL_NVP(Extent));
	};


};

CEREAL_REGISTER_TYPE(BoxCollider);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Collider, BoxCollider);