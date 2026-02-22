#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std libs
#include <memory>

//cereal
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/xml.hpp>
#include "Serialisation/DefinitionsKarnanCereal.h"

#include "BasicMesh.h"
#include "KarnanMaterial.h"
#include "Physics/Colliders.h"
#include "Scripting/ScriptComponent.h"


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

        glm::vec3 ForwardVector()
        {
            glm::vec3 forward;
            forward.x = cos(Rotation.x) * sin(Rotation.y);
            forward.y = sin(Rotation.x);
            forward.z = cos(Rotation.x) * cos(Rotation.y);

            return glm::normalize(forward);
        }

        glm::vec3 UpVector()
        {
            glm::vec4 yUp = { 0.0f, 1.0f, 0.0f, 0.0f };
            glm::vec3 rotated = glm::normalize(glm::vec3(yUp * Mat4()));
            return { rotated.x, rotated.y, rotated.z };
        }


        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(Translation));
            archive(CEREAL_NVP(Rotation));
            archive(CEREAL_NVP(Scale));
        }

	};

}

class GameObject 
{

public:
    Karnan::Transform Transform;
    std::string ObjectName;
    std::vector<std::string> Tags;

protected:
    uint32_t _objectId;

	KarnanDevice& _karnanDevice;

	bool _renderable = false;
    bool _meshRefreshed = false;
    std::string _meshName = "";
    std::shared_ptr<BasicMesh> _meshPointer = nullptr;

    std::string _materialName = "";
    std::shared_ptr<KarnanMaterial> _material = nullptr;
    bool _materialChanged = false;

    bool _colliderActive = false;
    std::shared_ptr<Collider> _collider;

    std::vector<std::shared_ptr<ScriptableComponent>> _components;

    bool _startCalled = false;

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
    bool HasTag(std::string tag);

    uint32_t GetId() const { return _objectId; };
    std::shared_ptr<KarnanMaterial> GetMaterial() { return _material; };
    std::string GetMeshName() { return _meshName; };
    std::string GetMaterialName() { return _materialName; };

    bool IsColliderActive() const { return _colliderActive; };
    void ChangeColliderActive(bool isActive) { _colliderActive = isActive; };
    std::shared_ptr<Collider> GetCollider() { return _collider; };
    void SetCollider(std::shared_ptr<Collider> newCollider) { _collider = newCollider; };

    virtual void Init();
    virtual void Start();
    virtual void Update(double deltaTime);
    virtual void Render(VkCommandBuffer commandBuffer);

    void CreateMesh(const std::string& filename);
    void CreateMaterial(const std::string& filename);
    void UpdateMaterial();

    void AddMaterial(const std::string& filename);

    void SetMeshRefreshed() { _meshRefreshed = true; };
    void SetGOName(std::string name) { ObjectName = name; };

    std::vector<std::shared_ptr<ScriptableComponent>>& GetComponents() { return _components; };
    void AddComponent(ScriptableComponent* component) 
    { 
        _components.push_back(std::shared_ptr<ScriptableComponent>(component));
        _components.back()->Init();
        _components.back()->SetGameobject(this);

    };
    void RemoveComponent(int index);
    ScriptableComponent* FindFirstComponentOfName(std::string componentName);

    void ResetStartCalled() { _startCalled = false; };


    //Cereal serialisation
    template <class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::make_nvp("Transform", Transform));
        ar(cereal::make_nvp("Object_Name", ObjectName));
        ar(cereal::make_nvp("Mesh_Name", _meshName));
        ar(cereal::make_nvp("Material_Name", _materialName));
        ar(cereal::make_nvp("Collider_active", _colliderActive));
        ar(cereal::make_nvp("Collider", _collider));
        ar(cereal::make_nvp("NumberComponents", _components.size()));
        for (int i = 0; i < _components.size(); i++)
        {
            ar(cereal::make_nvp("ScriptComponent", _components[i]));
        }
       
    };
    
    template <class Archive>
    void load(Archive& ar)
    {;
        ar(cereal::make_nvp("Transform", Transform));
        ar(cereal::make_nvp("Object_Name", ObjectName));
        ar(cereal::make_nvp("Mesh_Name", _meshName));
        ar(cereal::make_nvp("Material_Name", _materialName));
        ar(cereal::make_nvp("Collider_active", _colliderActive));
        ar(cereal::make_nvp("Collider", _collider));
        size_t numberComponents;
        ar(cereal::make_nvp("NumberComponents", numberComponents));
        _components.reserve(numberComponents);
        for (int i = 0; i < numberComponents; i++)
        {
            std::shared_ptr<ScriptableComponent> newComponent;
            ar(cereal::make_nvp("ScriptComponent", newComponent));
            _components.push_back(newComponent);
        }
    };

    template <class Archive>
    static void load_and_construct(Archive& ar, cereal::construct<GameObject>& construct)
    {
        std::string gameObjectName;
        ar(cereal::make_nvp("Object_Name", gameObjectName));
        construct(gameObjectName.c_str());

        Karnan::Transform transform;
        ar(cereal::make_nvp("Transform", transform));
        construct->Transform = transform;
        
        std::string meshName;
        ar(cereal::make_nvp("Mesh_Name", meshName));
        construct->_meshName = meshName;

        std::string materialName;
        ar(cereal::make_nvp("Material_Name", materialName));
        construct->_materialName = materialName;

        bool colliderActive;
        ar(cereal::make_nvp("Collider_active", colliderActive));
        construct->_colliderActive = colliderActive;

        std::shared_ptr<Collider> collider;
        ar(cereal::make_nvp("Collider", collider));
        construct->_collider = collider;


        size_t numberComponents;
        ar(cereal::make_nvp("NumberComponents", numberComponents));
        construct->_components.reserve(numberComponents);

        for (int i = 0; i < numberComponents; i++)
        {
            std::shared_ptr<ScriptableComponent> newComponent;
            ar(cereal::make_nvp("ScriptComponent", newComponent));
            construct->_components.push_back(newComponent);
        }


    }


private:


};

//CEREAL_REGISTER_TYPE(GameObject)
