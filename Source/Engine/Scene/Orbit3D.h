#pragma once

#include "Node3D.h"

namespace bl
{

class Orbit3D : public Node3D
{
    OBJECT_BOILER(Orbit3D, Node3D)
    float _minRadius;
    float _maxRadius;
    float _radius;
    float _azimuthAngle;
    float _polarAngle;

public:
    Orbit3D();
    Orbit3D(const Orbit3D& rhs);
    ~Orbit3D();

    virtual void Update(float dt) override;

    void SetMinRadius(float minRadius);
    void SetMaxRadius(float maxRadius);
    void SetRadius(float radius);
    void SetAzimuthAngle(float radians);
    void SetPolarAngle(float radians);

    float GetMinRadius();
    float GetMaxRadius();
    float GetRadius();
    float GetAzimuthAngle();
    float GetPolarAngle();

    void RotateAzimuth(float radians);
    void RotatePolar(float radians);
    void AddRadius(float radius);

    static void RegisterClass();

};

}