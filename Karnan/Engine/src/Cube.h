#pragma once
#include "GameObject.h"

class Cube : public GameObject
{
public:

private:

public:
	Cube(const char* objectName);
	~Cube();

	void Update(double deltaTime) override;

private:

};