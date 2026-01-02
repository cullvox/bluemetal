#include "Camera3D.h"
#include "Engine/Engine.h"
#include "Window/Window.h"

namespace bl {

Camera3D::Camera3D(Engine& engine)
    : Node3D(engine)
    , _projection(CameraProjection::ePerspective)
    , _nearClip(0.01f)
    , _farClip(1000.0f)
    , _fov(85.0f)
    , _projectionMatrix(glm::perspective(glm::radians(_fov), 1.77778f, _nearClip, _farClip))
{
}

Camera3D::~Camera3D()
{
}

void Camera3D::SetProjection(CameraProjection projection)
{
    _projection = projection;
    _isDirty = true;
}

void Camera3D::SetNearClip(float near)
{
    _nearClip = near;
    _isDirty = true;
}

void Camera3D::SetFarClip(float far)
{
    _farClip = far;
    _isDirty = true;
}

void Camera3D::SetFOV(float fov)
{
    _fov = fov;
    _isDirty = true;
}

const glm::mat4& Camera3D::GetProjectionMatrix()
{
    if (!_isDirty) {
        return _projectionMatrix;
    }

    Extent2D extent = GetEngine().GetWindow()->GetExtent();
    switch (_projection) {
    case CameraProjection::ePerspective:
        _projectionMatrix = glm::perspective(glm::radians(_fov), (float)extent.width / (float)extent.height, _nearClip, _farClip);
        break;
    case CameraProjection::eOrthographic:
        _projectionMatrix = glm::ortho((float)extent.height, (float)extent.height, (float)extent.width, (float)extent.width, _nearClip, _farClip);
        break;
    }

    _projectionMatrix[1][1] *= -1; // Invert the projection for Vulkan y (0, 1)

    _isDirty = false;
    return _projectionMatrix;
}

const glm::mat4& Camera3D::GetViewMatrix()
{
    _viewMatrix = glm::inverse(GetWorldMatrix());
    return _viewMatrix;
}

} // namespace bl