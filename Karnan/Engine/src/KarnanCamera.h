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

	float _aspect = 1.0f;

	float _fovy = 90.0f;
	float _near = 0.1f;
	float _far = 10000.0f;


public:
	KarnanCamera(const char* objectName);


	void Init() override;
	void Start() override;
	void Update(double deltaTime) override;

	void SetProjection(float aspect);
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


	float GetFOV() { return _fovy; };
	void SetFOV(float fov) { _fovy = fov; };
	
	float GetNearPlaneDistance() { return _near; };
	void SetNearPlaneDistance(float near) { _near = near; };

	float GetFarPlaneDistance() { return _far; };
	void SetFarPlaneDistance(float far) { _far = far; };

	void RemoveMainCameraTag();
	void AddMainCameraTag() { Tags.push_back("Main Camera"); };

	//Cereal serialisation
	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<GameObject>(this));
		ar(CEREAL_NVP(_aspect));
	};

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<GameObject>(this));
		ar(CEREAL_NVP(_aspect));
	};

protected:
	KarnanCamera() 
		: GameObject("Weewooo"), _aspect(60)
	{

	};

	friend class cereal::access;


private:



};

#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(KarnanCamera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObject, KarnanCamera);