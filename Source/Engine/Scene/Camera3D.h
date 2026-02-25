#pragma once

#include "Node3D.h"

namespace bl {

enum class CameraProjection {
    ePerspective,
    eOrthographic
};

class Camera3D : public Node3D {
    CameraProjection _projection;
    float _fov;
    float _nearClip;
    float _farClip;
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    float _aspect;
    bool _isDirty;

public:
    Camera3D(Engine& engine);
    ~Camera3D();

    void SetProjection(CameraProjection projection);
    void SetNearClip(float near);
    void SetFarClip(float far);
    void SetFOV(float fov);

    const glm::mat4& GetProjectionMatrix();
    const glm::mat4& GetViewMatrix();
};

} // namespace bl