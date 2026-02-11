#include "KarnanPhysics.h"

#include "../SceneManagement/KarnanScene.h"
#include "../GameObject.h"
#include "Colliders.h"

KarnanPhysics::KarnanPhysics()
{
}

KarnanPhysics::~KarnanPhysics()
{
}

void KarnanPhysics::UpdatePhysics(KarnanScene* scene)
{
    ClearEvents();
    std::vector<std::shared_ptr<GameObject>> gameObjects = scene->GetAllGameObjects();
    for (auto gameObject : gameObjects)
    {
        if (!gameObject->IsColliderActive())
            continue;
        
        //todo add gameObject to Spatial Partition
    }

    for (auto gameObject : gameObjects)
    {
        if (!gameObject->IsColliderActive())
            continue;
        for (auto otherGameObject : gameObjects)
        {
            if (!otherGameObject->IsColliderActive())
                continue;
            if (gameObject->GetId() == otherGameObject->GetId())
                continue;
            if (CheckIntersect(gameObject->GetCollider().get(), otherGameObject->GetCollider().get()))
            {
                _collisionEvents.push_back({ gameObject, otherGameObject });
            }
        }
    }
}

bool KarnanPhysics::CheckIntersect(Collider* colA, Collider* colB)
{
    if (colA->Type == Collider::ColliderType::SPHERE && colB->Type == Collider::ColliderType::SPHERE)
    {
        SphereCollider* sphereA = static_cast<SphereCollider*>(colA);
        SphereCollider* sphereB = static_cast<SphereCollider*>(colB);

        float minimumDistance = sphereA->Radius + sphereB->Radius;

        glm::vec3 AtoB = sphereB->Transform()[3] - sphereA->Transform()[3];
        float distance2 = glm::dot(AtoB, AtoB);

        return distance2 < minimumDistance * minimumDistance;
    }

    if (colA->Type == Collider::ColliderType::SPHERE && colB->Type == Collider::ColliderType::BOX)
    {
        SphereCollider* sphereA = static_cast<SphereCollider*>(colA);
        BoxCollider* boxB = static_cast<BoxCollider*>(colB);
        return BoxSphereIntersection(boxB, sphereA);
    }

    if (colA->Type == Collider::ColliderType::BOX && colB->Type == Collider::ColliderType::SPHERE)
    {
        BoxCollider* boxA = static_cast<BoxCollider*>(colA);
        SphereCollider* sphereB = static_cast<SphereCollider*>(colB);
        return BoxSphereIntersection(boxA, sphereB);
    }

    if (colA->Type == Collider::ColliderType::BOX && colB->Type == Collider::ColliderType::BOX)
    {
        BoxCollider* boxA = static_cast<BoxCollider*>(colA);
        BoxCollider* boxB = static_cast<BoxCollider*>(colB);
        return BoxBoxIntersection(boxA, boxB);
    }


    return false;
}

bool KarnanPhysics::BoxSphereIntersection(BoxCollider* boxA, SphereCollider* sphereB)
{
    glm::vec3 sphereCenter = glm::vec3(sphereB->Transform()[3]);
    glm::vec3 localSphereCenter = glm::inverse(boxA->ScalelessTransform()) * glm::vec4(sphereCenter, 1.0f);
    glm::vec3 closestPoint = glm::clamp(localSphereCenter, -boxA->Extent, boxA->Extent);
    glm::vec3 AtoB = localSphereCenter - closestPoint;
    float dist2 = glm::dot(AtoB, AtoB);
    return dist2 < sphereB->Radius * sphereB->Radius;
}

bool KarnanPhysics::BoxBoxIntersection(BoxCollider* boxA, BoxCollider* boxB)
{
    std::vector<glm::vec3> candidateAxises = GetCandidateAxises(boxA, boxB);

    for (int i = 0; i < candidateAxises.size(); i++)
    {
        glm::vec3 normalisedAxis = glm::normalize(candidateAxises[i]);

        float minA, maxA, minB, maxB;
        ProjectCubeAxis(boxA, normalisedAxis, minA, maxA);
        ProjectCubeAxis(boxB, normalisedAxis, minB, maxB);

        float overlap = glm::min(maxA, maxB) - glm::max(minA, minB);
        if (overlap < 0)
            return false;
     }

    return true;
}

void KarnanPhysics::ProjectCubeAxis(BoxCollider* box, glm::vec3 axis, float& min, float& max)
{
    glm::vec3 boxCenterPos = { box->Transform()[3][0], box->Transform()[3][1], box->Transform()[3][2] };
    float centerProjection = glm::dot(boxCenterPos, axis);

    std::vector<glm::vec3> cubeAxises = box->GetAxises();

    float radius =
        box->Extent.x * abs(glm::dot(cubeAxises[0], axis)) +
        box->Extent.y * abs(glm::dot(cubeAxises[1], axis)) +
        box->Extent.z * abs(glm::dot(cubeAxises[2], axis));

    min = centerProjection - radius;
    max = centerProjection + radius;
}

std::vector<glm::vec3> KarnanPhysics::GetCandidateAxises(BoxCollider* boxA, BoxCollider* boxB)
{
    std::vector<glm::vec3> axises;

    std::vector<glm::vec3> boxAAxises = boxA->GetAxises();
    std::vector<glm::vec3> boxBAxises = boxB->GetAxises();
    axises.insert(axises.end(), boxAAxises.begin(), boxAAxises.end());
    axises.insert(axises.end(), boxBAxises.begin(), boxBAxises.end());

    for (int i = 0; i < boxAAxises.size(); i++)
    {
        for (int j = 0; j < boxBAxises.size(); j++)
        {
            glm::vec3 axis = glm::cross(boxAAxises[i], boxBAxises[j]);
            float eps = 0.000001f;
            if (glm::dot(axis, axis) < eps)
                continue;
            axises.push_back(axis);
        }
    }
    return axises;
}
