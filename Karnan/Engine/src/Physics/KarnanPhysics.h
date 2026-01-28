#pragma once

#include <vector>

#include "../SceneManagement/KarnanScene.h"

class GameObject;

struct CollisionEvent
{
	GameObject* GameobjectA;
	GameObject* GameobjectB;
};

class KarnanPhysics
{
public:


private:


public:
	KarnanPhysics();
	~KarnanPhysics();

	void UpdatePhysics(KarnanScene* scene);
	
	std::vector<CollisionEvent>& GetCollisionEvents() { return _collisionEvents; };



private:
	std::vector<CollisionEvent> _collisionEvents;
	
};