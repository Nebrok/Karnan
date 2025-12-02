#pragma once

#include "BasicMesh.h"
#include "KarnanMaterial.h"

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
        glm::mat4 Mat4() const {
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
	    
        glm::mat3 NormalMatrix() const 
        {
            const float c3 = glm::cos(Rotation.z);
            const float s3 = glm::sin(Rotation.z);
            const float c2 = glm::cos(Rotation.x);
            const float s2 = glm::sin(Rotation.x);
            const float c1 = glm::cos(Rotation.y);
            const float s1 = glm::sin(Rotation.y);

            const glm::vec3 invScale = 1.0f / Scale;
            return glm::mat3
            {
                {
                    invScale.x * (c1 * c3 + s1 * s2 * s3),
                    invScale.x * (c2 * s3),
                    invScale.x * (c1 * s2 * s3 - c3 * s1),
                },
                {
                    invScale.y * (c3 * s1 * s2 - c1 * s3),
                    invScale.y * (c2 * c3),
                    invScale.y * (c1 * c3 * s2 + s1 * s3),
                },
                {
                    invScale.z * (c2 * s1),
                    invScale.z * (-s2),
                    invScale.z * (c1 * c2),
                }
            };
        }

	};

}

class GameObject 
{

public:
    Karnan::Transform Transform;

    const char* ObjectName;
    

protected:
    uint32_t _objectId;

	KarnanDevice& _karnanDevice;


	bool _renderable = false;
    bool _meshRefreshed = false;
    std::string _meshName = "";
    std::shared_ptr<BasicMesh> _meshPointer = nullptr;

    std::shared_ptr<KarnanMaterial> _material = nullptr;

public:
    static uint32_t GenerateNewId()
    {
        static uint32_t currentId = 0;
        return currentId++;
    }

	GameObject(const char* objectName);
	~GameObject();

    void SetRenderable(bool renderable) { _renderable = renderable; };
    bool IsRenderable() const { return _renderable; };

    uint32_t GetId() const { return _objectId; };
    std::shared_ptr<KarnanMaterial> GetMaterial() { return _material; };

    virtual void Init();
    virtual void Update(double deltaTime);
    virtual void Render(VkCommandBuffer commandBuffer);
    
    void CreateMesh(const std::string& filename);
    void CreateMaterial(const std::string& filename);

    void SetMeshRefreshed() { _meshRefreshed = true; };


private:


};