#pragma once

#include "GameObject.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>



class KarnanCamera : public GameObject
{
public:

private:
	glm::mat4 _projectionMatrix{ 1.f };
	glm::mat4 _viewMatrix{ 1.f };


	float _lookSpeed = 1.0f;
	float _moveSpeed = 1.0f;

	float _aspect = 60.0f;

public:
	KarnanCamera(const char* objectName, glm::vec3 position);

	void Update(double deltaTime) override;

	void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

	void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

	void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
	void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
	void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

	const glm::mat4 GetProjection() const
	{
		return _projectionMatrix;
	}
	const glm::mat4 GetView() const
	{
		return _viewMatrix;
	}


	//Cereal serialisation
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(CEREAL_NVP(cereal::base_class<GameObject>(this)));
		ar(CEREAL_NVP(_aspect));
	};

	template <class Archive>
	static void load_and_construct(Archive& ar, cereal::construct<KarnanCamera>& construct)
	{
		std::string gameObjectName;
		glm::vec3 transform;
		ar(cereal::make_nvp("Object Name", gameObjectName));
		ar(cereal::make_nvp("Transform", transform));
		construct(gameObjectName.c_str() , transform);
	}

private:



};

#include <cereal/archives/json.hpp>

CEREAL_REGISTER_TYPE(KarnanCamera);