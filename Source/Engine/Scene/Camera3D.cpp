#include "Camera3D.h"
#include "Engine/Engine.h"

namespace bl {

Camera3D::Camera3D(Engine& engine)
    : Node3D(engine)
{
    _projection = CameraProjection::ePerspective;
    _nearClip = 0.01f;
    _farClip = 1000.0f;
    _fov = 85.0f;
}

void Camera3D::SetProjection(CameraProjection projection)
{
    _projection = projection;
}

void Camera3D::SetNearClip(float near)
{
    _nearClip = near;
}

void Camera3D::SetFarClip(float far)
{
    _farClip = far;
}

void Camera3D::SetFOV(float fov)
{
    _fov = fov;
}

glm::mat4 Camera3D::GetProjectionMatrix()
{
    Extent2D extent = GetEngine().GetWindow()->GetExtent();
    switch (_projection) {
    case CameraProjection::ePerspective:
        return glm::perspectiveFov(_fov, (float)extent.width, (float)extent.height, _nearClip, _farClip);
        break;
    case CameraProjection::eOrthographic:
        return glm::ortho((float)extent.height, (float)extent.height, (float)extent.width, (float)extent.width, _nearClip, _farClip);
        break;
    }

    return glm::identity<glm::mat4>();
}

glm::mat4 Camera3D::GetViewMatrix()
{
    return GetTransform();
}

} // namespace bl