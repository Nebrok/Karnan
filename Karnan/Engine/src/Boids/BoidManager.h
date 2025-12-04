#pragma once

#include "../KarnanDevice.h"
#include "../GameObject.h"

#include "Boid.h"


class BoidManager
{
private:
	int _numberBoids = 150;

	float _separationDistance = 1.35f;
	float _alignmentDistance = 10.0f;
	float _cohesionDistance = 6.f;

	float _separationForceScalar = 2.6f;
	float _alignmentForceScalar = 1.f;
	float _cohesionForceScalar = .3f;

	std::vector<Boid*> _boids;

public:
	BoidManager();
	~BoidManager();

	std::vector<GameObject*> GetGameObjects();


	void Separation();
	void Alignment();
	void Cohesion();

	void UpdateBoids(float deltaTime);

private:



};