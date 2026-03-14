#include "Orbit3D.h"

//ref: https://www.mbsoftworks.sk/tutorials/opengl4/026-camera-pt3-orbit-camera/

namespace bl
{

Orbit3D::Orbit3D(Engine& engine)
    : Node3D(engine)
{
}

Orbit3D::Orbit3D(const Orbit3D& rhs)
    : Node3D(rhs)
    , _radius(rhs._radius)
    , _up(rhs._up)
    , _azimuthAngle(rhs._azimuthAngle)
    , _polarAngle(rhs._polarAngle)
{
}

Orbit3D::~Orbit3D()
{
}

Orbit3D* Orbit3D::Clone()
{
    return new Orbit3D(*this);
}

void Orbit3D::Update(float)
{
    const float sineAzimuth = sin(_azimuthAngle);
    const float cosineAzimuth = cos(_azimuthAngle);
    const float sinePolar = sin(_polarAngle);
    const float cosinePolar = cos(_polarAngle);

    const float x = _radius * cosinePolar * cosineAzimuth;
    const float y = _radius * sinePolar;
    const float z = _radius * cosinePolar * sineAzimuth;
    const glm::vec3 position = glm::vec3{x,y,z};

    SetPosition(position);

    const glm::vec3 forward = glm::normalize(-position);
    SetRotation(glm::quatLookAt(forward, _up));
}

void Orbit3D::RotateAzimuth(float radians)
{
    _azimuthAngle += radians;

    constexpr float fullCircle = 2.0f * glm::pi<float>();
    _azimuthAngle = fmodf(_azimuthAngle, fullCircle);

    if (_azimuthAngle < 0.0f)
    {
        _azimuthAngle = fullCircle + _azimuthAngle;
    }
}

void Orbit3D::RotatePolar(float radians)
{
    _polarAngle += radians;

    const float polarCap = glm::pi<float>() / 2.0f - 0.01f;

    if (_polarAngle > polarCap)
    {
        _polarAngle = polarCap;
    }

    if (_polarAngle < -polarCap)
    {
        _polarAngle = -polarCap;
    }
}

void Orbit3D::SetMaxRadius(float maxRadius)
{
    _maxRadius = maxRadius;
}

void Orbit3D::AddRadius(float radius)
{
    _radius += radius;

    if (_radius <= 0.001)
    {
        _radius = 0.001;
    }

    if (_radius >= _maxRadius)
    {
        _radius = _maxRadius;
    }
}

} // namespace bl