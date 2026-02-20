#include "KarnanCamera.h"
#include "InputManagementSystem.h"

#include <cassert>
#include <limits>
#include <glm/gtc/constants.hpp>

#include "EngineCore.h"

KarnanCamera::KarnanCamera(const char* objectName)
    : GameObject(objectName)
{
    _renderable = false;
}

void KarnanCamera::Init()
{
    _renderable = false;
    Tags.push_back("Main Camera");
    Tags.push_back("Camera");
}

void KarnanCamera::Start()
{
}

void KarnanCamera::Update(double deltaTime)
{
    if (EngineCore::Instance->PlayMode())
    {
        SetViewYXZ(Transform.Translation, Transform.Rotation);
        return;
    }

    glm::vec3 rotationDelta = { 0.0f, 0.0f, 0.0f };
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_left))
        rotationDelta.y--;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_right))
        rotationDelta.y++;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_up))
        rotationDelta.x--;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::arrow_down))
        rotationDelta.x++;

    if (glm::dot(rotationDelta, rotationDelta) > std::numeric_limits<float>::epsilon())
    {
        Transform.Rotation += _lookSpeed * (float)deltaTime * glm::normalize(rotationDelta);
    }

    //limit pitch values between about +/- 85ish degrees
    //Transform.Rotation.x = glm::clamp(Transform.Rotation.x, -1.5f, 1.f);
    Transform.Rotation.y = glm::mod(Transform.Rotation.y, glm::two_pi<float>());

    float pitch = Transform.Rotation.z;
    float yaw = Transform.Rotation.y;
    const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir = glm::normalize(glm::cross(-forwardDir, {0, 1.0f, 0}));
    const glm::vec3 upDir = { 0.0f, 1.0f, 0.0f };

    glm::vec3 moveDir{ 0.0f, 0.0f, 0.0f };

    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::w))
        moveDir += forwardDir;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::s))
        moveDir -= forwardDir;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::a))
        moveDir -= rightDir;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::d))
        moveDir += rightDir;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::q))
        moveDir += upDir;
    if (InputManagementSystem::Instance->GetKeyPressed(InputManagementSystem::SupportedKeys::e))
        moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
    {
       Transform.Translation += _moveSpeed * (float)deltaTime * glm::normalize(moveDir);
    }

    _moveSpeed += InputManagementSystem::Instance->GetScrollDelta().y;
    if (InputManagementSystem::Instance->GetScrollDelta().y != 0)
    {
        int waaa = 1;
    }

    SetViewYXZ(Transform.Translation, Transform.Rotation);
}

void KarnanCamera::SetProjection(float aspect)
{
    SetViewYXZ(Transform.Translation, Transform.Rotation);
    SetPerspectiveProjection(glm::radians(_fovy), aspect, _near, _far);
}

void KarnanCamera::SetOrthographicProjection(
    float left, float right, float top, float bottom, float near, float far) {
    _projectionMatrix = glm::mat4{ 1.0f };
    _projectionMatrix[0][0] = 2.f / (right - left);
    _projectionMatrix[1][1] = 2.f / (bottom - top);
    _projectionMatrix[2][2] = 1.f / (far - near);
    _projectionMatrix[3][0] = -(right + left) / (right - left);
    _projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    _projectionMatrix[3][2] = -near / (far - near);
}

void KarnanCamera::SetPerspectiveProjection(float fovy, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovy / 2.f);
    _projectionMatrix = glm::mat4{ 0.0f };
    _projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    _projectionMatrix[1][1] = 1.f / (tanHalfFovy) * -1;
    _projectionMatrix[2][2] = far / (far - near);
    _projectionMatrix[2][3] = 1.f;
    _projectionMatrix[3][2] = -(far * near) / (far - near);
}

void KarnanCamera::SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    const glm::vec3 w{ glm::normalize(direction) };
    const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
    const glm::vec3 v{ glm::cross(w, u) };

    _viewMatrix = glm::mat4{ 1.f };
    _viewMatrix[0][0] = u.x;
    _viewMatrix[1][0] = u.y;
    _viewMatrix[2][0] = u.z;
    _viewMatrix[0][1] = v.x;
    _viewMatrix[1][1] = v.y;
    _viewMatrix[2][1] = v.z;
    _viewMatrix[0][2] = w.x;
    _viewMatrix[1][2] = w.y;
    _viewMatrix[2][2] = w.z;
    _viewMatrix[3][0] = -glm::dot(u, position);
    _viewMatrix[3][1] = -glm::dot(v, position);
    _viewMatrix[3][2] = -glm::dot(w, position);
}

void KarnanCamera::SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    SetViewDirection(position, target - position, up);
}

void KarnanCamera::SetViewYXZ(glm::vec3 position, glm::vec3 rotation) {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
    const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
    const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
    _viewMatrix = glm::mat4{ 1.f };
    _viewMatrix[0][0] = u.x;
    _viewMatrix[1][0] = u.y;
    _viewMatrix[2][0] = u.z;
    _viewMatrix[0][1] = v.x;
    _viewMatrix[1][1] = v.y;
    _viewMatrix[2][1] = v.z;
    _viewMatrix[0][2] = w.x;
    _viewMatrix[1][2] = w.y;
    _viewMatrix[2][2] = w.z;
    _viewMatrix[3][0] = -glm::dot(u, position);
    _viewMatrix[3][1] = -glm::dot(v, position);
    _viewMatrix[3][2] = -glm::dot(w, position);
}

void KarnanCamera::RemoveMainCameraTag()
{
    int index = 0;
    for (std::string tag : Tags)
    {
        if (tag.compare("Main Camera") == 0)
        {
            break;
        }
        index++;
    }
    Tags.erase(Tags.begin()+index);
}
