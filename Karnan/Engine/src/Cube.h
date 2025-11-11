#pragma once
#include "GameObject.h"

class Cube : public GameObject
{
public:

private:

public:
	Cube(KarnanDevice& device);
	~Cube();

	void Update(double deltaTime) override;

private:

};