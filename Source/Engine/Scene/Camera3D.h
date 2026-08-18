#pragma once

#include "Node3D.h"

namespace bl {

enum class CameraProjection : int64_t {
    ePerspective,
    eOrthographic
};

class Camera3D : public Node3D {
    OBJECT_BOILER(Camera3D, Node3D)

    CameraProjection _projection;
    float _fov;
    float _nearClip;
    float _farClip;
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    float _aspect;
    bool _isDirty;

public:
    Camera3D();
    ~Camera3D();

    void SetProjection(CameraProjection projection);
    CameraProjection GetProjection();
    void SetFOV(float fov);
    float GetFOV();
    void SetNearClip(float near);
    float GetNearClip();
    void SetFarClip(float far);
    float GetFarClip();

    const glm::mat4& GetProjectionMatrix();
    const glm::mat4& GetViewMatrix();

    static void RegisterClass();
};

} // namespace bl