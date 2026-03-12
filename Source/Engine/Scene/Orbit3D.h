#pragma once

#include "Node3D.h"

namespace bl
{

class Orbit3D : public Node3D
{
    float _radius = 10.0f;
    glm::vec3 _up = glm::vec3{0.0f, 1.0f, 0.0f};
    float _azimuthAngle = glm::radians(0.0f);
    float _polarAngle = glm::radians(0.0f);

public:
    Orbit3D(Engine& engine);
    Orbit3D(const Orbit3D& rhs);
    ~Orbit3D();

    virtual Orbit3D* Clone() override;
    virtual void Update(float dt) override;

    void RotateAzimuth(float radians);
    void RotatePolar(float radians);

};

}