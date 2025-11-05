#pragma once

#include "Node3D.h"

namespace bl
{

enum class CameraProjection
{
    ePerspective,
    eOrthographic
};

class Camera3D : public Node3D
{
    CameraProjection _projection;
    float _fov;
    float _nearClip;
    float _farClip;

public:
    Camera3D(Engine* engine);
    ~Camera3D();

    void SetProjection(CameraProjection projection);
    void SetNearClip(float near);
    void SetFarClip(float far);
    void SetFOV(float fov);

    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetViewMatrix();
};

} // namespace bl