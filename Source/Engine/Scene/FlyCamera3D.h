#pragma once

#include "Camera3D.h"

namespace bl {

/** @brief A camera with input controls for flying through a scene. */
class FlyCamera3D : public Camera3D {
    float _speed;
    float _smooth;

public:
    FlyCamera3D(Engine& engine);
    ~FlyCamera3D();

    virtual void Update(float dt);

    void SetSpeed(float speed);
    void SetSmoothness(float smoothness);
};

} // namespace bl