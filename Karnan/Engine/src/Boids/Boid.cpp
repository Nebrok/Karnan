#include "Boid.h"

#include <iostream>

Boid::Boid()
{
    _gameObject = DBG_NEW GameObject("Boid");
    _gameObject->CreateMesh("assets/cube.obj");
    _gameObject->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
    _gameObject->Transform.Translation = { 0.0f, 2.0f, 0.0f };
}

void Boid::Init()
{
    float xPos = (std::rand() % (BOUNDING_BOX_HALFWIDTH * 10)) / 10.f;
    float yPos = (std::rand() % (BOUNDING_BOX_HALFWIDTH * 10)) / 10.f;
    float zPos = (std::rand() % (BOUNDING_BOX_HALFWIDTH * 10)) / 10.f;
    _gameObject->Transform.Translation = { xPos, yPos, zPos };
    _gameObject->Transform.Scale = { 1.f, 1.f, 1.f };
}

Boid::~Boid()
{
    
}

void Boid::UpdateBoid(float deltaTime)
{
    _velocity += _acceleration;
    if (glm::length(_velocity) > _maxSpeed)
        _velocity = glm::normalize(_velocity) * _maxSpeed;
    _acceleration = { 0.f, 0.f, 0.f };

    if (_gameObject->Transform.Translation.x > BOUNDING_BOX_HALFWIDTH)
    {
        float tempVal = _gameObject->Transform.Translation.x - BOUNDING_BOX_HALFWIDTH;
        _gameObject->Transform.Translation.x = -BOUNDING_BOX_HALFWIDTH + tempVal;
    }   
    else if (_gameObject->Transform.Translation.x < -BOUNDING_BOX_HALFWIDTH)
    {
        float tempVal = _gameObject->Transform.Translation.x + BOUNDING_BOX_HALFWIDTH;
        _gameObject->Transform.Translation.x = BOUNDING_BOX_HALFWIDTH + tempVal;
    }

    if (_gameObject->Transform.Translation.y > BOUNDING_BOX_HALFWIDTH)
    {
        float tempVal = _gameObject->Transform.Translation.y - BOUNDING_BOX_HALFWIDTH;
        _gameObject->Transform.Translation.y = -BOUNDING_BOX_HALFWIDTH + tempVal;
    }
    else if (_gameObject->Transform.Translation.y < -BOUNDING_BOX_HALFWIDTH)
    {
        float tempVal = _gameObject->Transform.Translation.y + BOUNDING_BOX_HALFWIDTH;
        _gameObject->Transform.Translation.y = BOUNDING_BOX_HALFWIDTH + tempVal;
    }

    if (_gameObject->Transform.Translation.z > BOUNDING_BOX_HALFWIDTH)
    {
        float tempVal = _gameObject->Transform.Translation.z - BOUNDING_BOX_HALFWIDTH;
        _gameObject->Transform.Translation.z = -BOUNDING_BOX_HALFWIDTH + tempVal;
    }
    else if (_gameObject->Transform.Translation.z < -BOUNDING_BOX_HALFWIDTH)
    {
        float tempVal = _gameObject->Transform.Translation.z + BOUNDING_BOX_HALFWIDTH;
        _gameObject->Transform.Translation.z = BOUNDING_BOX_HALFWIDTH + tempVal;
    }

    _gameObject->Transform.Translation += (_velocity * deltaTime);

}

void Boid::AddToVelocity(glm::vec3 velocity)
{
    _velocity += velocity;
    if (glm::length(_velocity) > _maxSpeed)
        _velocity = glm::normalize(_velocity) * _maxSpeed;
}

void Boid::SetVelocity(glm::vec3 velocity)
{
    _velocity = velocity;
    if (glm::length(_velocity) > _maxSpeed)
        _velocity = glm::normalize(_velocity) * _maxSpeed;
}

void Boid::AddToAcceleration(glm::vec3 acceleration)
{
    _acceleration += acceleration;
}

void Boid::SetAcceleration(glm::vec3 acceleration)
{
    _acceleration = acceleration;
}
