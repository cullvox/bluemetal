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
    Extent2D extent = GetEngine().GetWindow()->GetExtent();
    float width = static_cast<float>(extent.width);
    float height = static_cast<float>(extent.height);
    float aspect = width / height;

    if (_aspect != aspect) {
        _aspect = aspect;
        _isDirty = true;
    }

    if (!_isDirty) {
        return _projectionMatrix;
    }

    switch (_projection) {
    case CameraProjection::ePerspective:
        _projectionMatrix = glm::perspective(glm::radians(_fov), _aspect, _nearClip, _farClip);
        _projectionMatrix[1][1] *= -1; // Invert the projection for Vulkan y (0, 1)
        break;
    case CameraProjection::eOrthographic:
        _projectionMatrix = glm::ortho((-(width / 2.0f))/_fov, (width/2.0f)/_fov, (height/2.0f)/_fov, (-(height / 2.0f))/_fov, _nearClip, _farClip);
        break;
    }

    _isDirty = false;
    return _projectionMatrix;
}

const glm::mat4& Camera3D::GetViewMatrix()
{
    _viewMatrix = glm::inverse(GetWorldMatrix());
    return _viewMatrix;
}

} // namespace bl