#pragma once

#include "BasicMesh.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std libs
#include <memory>

namespace Karnan
{
	struct Transform
	{
		glm::vec3 Translation{ 0.f, 0.f, 0.f };
		glm::vec3 Rotation{ 0.f, 0.f, 0.f };
		glm::vec3 Scale{ 1.f, 1.f, 1.f };
		
        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 Mat4() {
            const float c3 = glm::cos(Rotation.z);
            const float s3 = glm::sin(Rotation.z);
            const float c2 = glm::cos(Rotation.x);
            const float s2 = glm::sin(Rotation.x);
            const float c1 = glm::cos(Rotation.y);
            const float s1 = glm::sin(Rotation.y);
            return glm::mat4{
                {
                    Scale.x * (c1 * c3 + s1 * s2 * s3),
                    Scale.x * (c2 * s3),
                    Scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    Scale.y * (c3 * s1 * s2 - c1 * s3),
                    Scale.y * (c2 * c3),
                    Scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    Scale.z * (c2 * s1),
                    Scale.z * (-s2),
                    Scale.z * (c1 * c2),
                    0.0f,
                },
                {Translation.x, Translation.y, Translation.z, 1.0f} };
        }
	
	};

}

class GameObject 
{

public:
    Karnan::Transform Transform;

protected:
	KarnanDevice& _karnanDevice;


	std::unique_ptr<BasicMesh> _mesh = nullptr;
	bool _renderable = false;


public:
	GameObject(KarnanDevice& device);
	~GameObject();

	void Init();
	void Update(float deltaTime);
	void Render(VkCommandBuffer commandBuffer);


private:


};