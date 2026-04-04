#pragma once

#include "Math/Math.h"
#include "Camera3D.h"

namespace bl {

/** @brief A camera with input controls for flying through a scene. */
class FlyCamera3D : public Camera3D {
    float _speed;
    float _smooth;
    float cameraAcceleration = 015.f;
    float maxCameraSpeed = 8.f;
    bool enableCameraSmoothing = true;
    bool enableCameraMovementDamping = true;
    float cameraSmoothnessDampLambda = 5.0f;
    float cameraMovementDampLambda = 5.0f;
    glm::vec3 cameraVelocity = { 0.0f, 0.0f, 0.0f };
    glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
    glm::vec3 cameraUp = { 0.0f, 1.0f, 0.0f };
    glm::mat4 view = glm::identity<glm::mat4>();
    float yaw = -90.0f, pitch = 0.0f;
    float yawVelocity = 0.0f, pitchVelocity = 0.0f;
    glm::vec3 direction;


public:
    FlyCamera3D(Engine& engine);
    ~FlyCamera3D();

    virtual void Update(float dt) override;
    virtual void Draw(RenderData& rd) override;

    void SetSpeed(float speed);
    float GetSpeed();
    void SetSmoothness(float smoothness);
    float GetSmoothness();

    static void RegisterClass(ClassDB& db);
};

} // namespace bl