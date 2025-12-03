#include "BoidManager.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

BoidManager::BoidManager()
{
    for (int i = 0; i < _numberBoids; i++)
    {
        Boid* boid = DBG_NEW Boid();
        boid->Init();
        boid->AddToVelocity(glm::vec3{ (std::rand() % 10) / 2.f, (std::rand() % 10) / 2.f , (std::rand() % 10) / 2.f });
        _boids.push_back(boid);
    }
}

BoidManager::~BoidManager()
{
    for (Boid* boid : _boids)
    {
        delete(boid);
    }
}

std::vector<GameObject*> BoidManager::GetGameObjects()
{
    std::vector<GameObject*> gos;
    for (auto& boid : _boids)
    {
        gos.push_back(boid->GetGameObject());
    }
    return gos;
}

void BoidManager::Separation()
{
    glm::vec3 dir{0.f, 0.f, 0.f};
    int total = 0;

    for (auto& boid1 : _boids)
    {
        dir = { 0.f, 0.f, 0.f };
        auto gameObj1 = boid1->GetGameObject();
        for (auto& boid2 : _boids)
        {
            auto gameObj2 = boid2->GetGameObject();
            if (gameObj1->GetId() == gameObj2->GetId())
                continue;
            
            glm::vec3 vecBetween = gameObj1->Transform.Translation - gameObj2->Transform.Translation;
            float dist = glm::length(vecBetween);

            if (dist >= _separationDistance || dist == 0)
            {
                continue;
            }
            total++;
            dir += vecBetween / (dist * dist);

        }
        if (total > 0)
        {
            dir = dir / (float)total;

            boid1->AddToAcceleration(dir * _separationForceScalar);
            total = 0;
        }
    }
}

void BoidManager::Alignment()
{
    glm::vec3 dir{ 0.f, 0.f, 0.f };
    int total = 0;

    for (auto& boid1 : _boids)
    {
        dir = { 0.f, 0.f, 0.f };
        auto gameObj1 = boid1->GetGameObject();
        for (auto& boid2 : _boids)
        {
            auto gameObj2 = boid2->GetGameObject();
            if (gameObj1->GetId() == gameObj2->GetId())
                continue;

            glm::vec3 vecBetween = gameObj1->Transform.Translation - gameObj2->Transform.Translation;
            float dist = glm::length(vecBetween);

            if (dist >= _alignmentDistance || dist == 0)
            {
                continue;
            }
            total++;
            dir += boid2->GetVelocity() / (dist * dist);

        }
        if (total > 0)
        {
            dir = dir / (float)total;

            boid1->AddToAcceleration(dir * _alignmentForceScalar);
            total = 0;
        }
    }
}

void BoidManager::Cohesion()
{
    std::vector<Boid*> nearby;
    for (auto& boid1 : _boids)
    {
        auto gameObj1 = boid1->GetGameObject();
        for (auto& boid2 : _boids)
        {
            auto gameObj2 = boid2->GetGameObject();
            if (gameObj1->GetId() == gameObj2->GetId())
                continue;

            glm::vec3 vecBetween = gameObj2->Transform.Translation - gameObj1->Transform.Translation;
            float dist = glm::length(vecBetween);

            if (dist <= _cohesionDistance && dist != 0)
            {
                nearby.push_back(boid2);
            }
        }

        if (nearby.size() <= 0)
            continue;

        glm::vec3 centre{0.f, 0.f, 0.f};
        for (auto neighbour : nearby)
        {
            centre += neighbour->GetGameObject()->Transform.Translation;
        }
        centre = centre / ((float)nearby.size());

        glm::vec3 dir = centre - gameObj1->Transform.Translation;

        boid1->AddToVelocity(dir * _cohesionForceScalar);
        nearby.clear();
    }
}

void BoidManager::UpdateBoids(float deltaTime)
{
    Separation();
    Alignment();
    Cohesion();
    for (auto& boid : _boids)
    {
        boid->UpdateBoid(deltaTime);
    }
}
