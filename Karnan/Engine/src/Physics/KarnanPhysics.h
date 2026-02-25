#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

class GameObject;
class Collider;
class SphereCollider;
class BoxCollider;
class KarnanScene;

struct CollisionEvent
{
	std::shared_ptr<GameObject> GameobjectA;
	std::shared_ptr<GameObject> GameobjectB;
	bool HasCollisionPoint = false;
	glm::vec3 Collision = { 0.0f, 0.0f, 0.0f };
};

struct RayCastHit
{
	std::shared_ptr<GameObject> GameobjectA;
	float rayLength;
	glm::vec3 CollisionPoint = { 0.0f, 0.0f, 0.0f };

};

class KarnanPhysics
{
public:


private:
	std::vector<CollisionEvent> _collisionEvents;


public:
	KarnanPhysics();
	~KarnanPhysics();

	void UpdatePhysics(KarnanScene* scene);
	static bool CheckIntersect(CollisionEvent& collisionEvent);
	static RayCastHit Raycast(KarnanScene* scene, glm::vec3 origin, glm::vec3 direction, float maxDistance, bool ignoreInternalStart);

	std::vector<CollisionEvent>& GetCollisionEvents() { return _collisionEvents; };
	void ClearEvents() { _collisionEvents.clear(); };


private:
	
	static bool BoxSphereIntersection(BoxCollider* boxA, SphereCollider* sphereB, glm::vec3& collisionPoint);
	
	static bool BoxBoxIntersection(BoxCollider* boxA, BoxCollider* sphereB);
	static void ProjectCubeAxis(BoxCollider* box, glm::vec3 axis, float& min, float& max);
	static std::vector<glm::vec3> GetCandidateAxises(BoxCollider* boxA, BoxCollider* boxB);

	static void GetAllActiveColliders(KarnanScene* scene, std::vector<std::shared_ptr<GameObject>>& gameObjects);
	static bool RaySlabIntersect(float origin, float direction, float minb, float maxb, float& tmin, float& tmax);

};