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
	static bool CheckIntersect(Collider* colA, Collider* colB);

	std::vector<CollisionEvent>& GetCollisionEvents() { return _collisionEvents; };
	void ClearEvents() { _collisionEvents.clear(); };


private:
	
	static bool BoxSphereIntersection(BoxCollider* boxA, SphereCollider* sphereB);
	
	static bool BoxBoxIntersection(BoxCollider* boxA, BoxCollider* sphereB);
	void ProjectCubeAxis(BoxCollider* box, glm::vec3 axis, float& min, float& max);


};