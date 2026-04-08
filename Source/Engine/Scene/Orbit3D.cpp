#include "Orbit3D.h"
#include "Core/ClassDB.h"

//ref: https://www.mbsoftworks.sk/tutorials/opengl4/026-camera-pt3-orbit-camera/

namespace bl
{

Orbit3D::Orbit3D(Engine& engine)
    : Node3D(engine)
    , _minRadius(0.1f)
    , _maxRadius(50.0f)
    , _radius(10.0f)
    , _azimuthAngle(0.0f)
    , _polarAngle(0.0f)
{
}

Orbit3D::Orbit3D(const Orbit3D& rhs)
    : Node3D(rhs)
    , _minRadius(0.1f)
    , _maxRadius(50.0f)
    , _radius(rhs._radius)
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
    SetRotation(glm::quatLookAt(forward, {0.0f, 1.0f, 0.0f}));
}

void Orbit3D::SetMinRadius(float minRadius)
{
    _minRadius = minRadius;
}

void Orbit3D::SetMaxRadius(float maxRadius)
{
    _maxRadius = maxRadius;
}

void Orbit3D::SetRadius(float radius)
{
    _radius = std::clamp(radius, _minRadius, _maxRadius);
}

void Orbit3D::SetAzimuthAngle(float radians)
{
    _azimuthAngle = radians;
}

void Orbit3D::SetPolarAngle(float radians)
{
    _polarAngle = radians;
}

float Orbit3D::GetMinRadius()
{
    return _minRadius;
}

float Orbit3D::GetMaxRadius()
{
    return _maxRadius;
}

float Orbit3D::GetRadius()
{
    return _radius;
}

float Orbit3D::GetAzimuthAngle()
{
    return _azimuthAngle;
}

float Orbit3D::GetPolarAngle()
{
    return _polarAngle;
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

void Orbit3D::AddRadius(float radius)
{
    _radius += radius;

    if (_radius <= 0.001f)
    {
        _radius = 0.001f;
    }

    if (_radius >= _maxRadius)
    {
        _radius = _maxRadius;
    }
}

void Orbit3D::RegisterClass(ClassDB& db)
{
    db.RegisterClass("Orbit3D", &Orbit3D::Create);
    db.RegisterProperty("Orbit3D", std::make_unique<TProperty<Orbit3D, float>>(db, "minRadius", &Orbit3D::SetMinRadius, &Orbit3D::GetMinRadius));
    db.RegisterProperty("Orbit3D", std::make_unique<TProperty<Orbit3D, float>>(db, "maxRadius", &Orbit3D::SetMaxRadius, &Orbit3D::GetMaxRadius));
    db.RegisterProperty("Orbit3D", std::make_unique<TProperty<Orbit3D, float>>(db, "radius", &Orbit3D::SetRadius, &Orbit3D::GetRadius));
    db.RegisterProperty("Orbit3D", std::make_unique<TProperty<Orbit3D, float>>(db, "azimuthAngle", &Orbit3D::SetAzimuthAngle, &Orbit3D::GetAzimuthAngle));
    db.RegisterProperty("Orbit3D", std::make_unique<TProperty<Orbit3D, float>>(db, "polarAngle", &Orbit3D::SetPolarAngle, &Orbit3D::GetPolarAngle));
}

} // namespace bl