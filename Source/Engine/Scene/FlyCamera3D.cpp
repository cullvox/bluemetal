#include "FlyCamera3D.h"
#include "Engine/Engine.h"
#include "Window/Input.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"
#include "Graphics/GraphicsSystem.h"
#include "Core/ClassDB.h"

namespace bl {

FlyCamera3D::FlyCamera3D(Engine& engine)
    : Camera3D(engine)
{
}

FlyCamera3D::~FlyCamera3D()
{
}

void FlyCamera3D::Update(float dt)
{
    auto& keyboard = GetEngine().GetInput()->GetKeyboard();
    auto& mouse = GetEngine().GetInput()->GetMouse();
    auto window = GetEngine().GetWindow();

    glm::vec3 position = GetPosition();

    // Compute camera velocity
    bool cameraMoved = false;
    glm::vec3 acceleration = glm::zero<glm::vec3>();
    if (keyboard.GetKeyDown(bl::Scancode::W)) {
        acceleration += cameraFront;
        cameraMoved = true;
    }
    if (keyboard.GetKeyDown(bl::Scancode::S)) {
        acceleration -= cameraFront;
        cameraMoved = true;
    }
    if (keyboard.GetKeyDown(bl::Scancode::A)) {
        acceleration -= glm::normalize(glm::cross(cameraFront, cameraUp));
        cameraMoved = true;
    }
    if (keyboard.GetKeyDown(bl::Scancode::D)) {
        acceleration += glm::normalize(glm::cross(cameraFront, cameraUp));
        cameraMoved = true;
    }
    if (keyboard.GetKeyDown(bl::Scancode::Space)) {
        acceleration += cameraUp;
        cameraMoved = true;
    }
    if (keyboard.GetKeyDown(bl::Scancode::LeftShift)) {
        acceleration -= cameraUp;
        cameraMoved = true;
    }

    if (glm::length(acceleration) > 0.0f) {
        acceleration = glm::normalize(acceleration) * cameraAcceleration;
    }

    cameraVelocity += acceleration * dt;

    // Apply camera velocity
    if (enableCameraMovementDamping) {
        position += cameraVelocity * dt;

        float cameraSpeed = glm::length(cameraVelocity);
        if (cameraSpeed > maxCameraSpeed) {
            cameraVelocity = glm::normalize(cameraVelocity) * maxCameraSpeed;
        }
    } else {
        // Treat acceleration as direct position change
        position += acceleration * dt;
    }

    SetPosition(position);

    // Apply friction to camera velocity
    if (enableCameraMovementDamping && !cameraMoved) {
        cameraVelocity = bl::DampExponential(cameraVelocity, glm::zero<glm::vec3>(), cameraMovementDampLambda, dt);
    } else if (!cameraMoved) {
        cameraVelocity = glm::zero<glm::vec3>();
    }

    //auto mousePos = mouse.GetMousePosition();
    auto mouseDelta = mouse.GetMouseDelta();

    // Update camera direction based on mouse movement, use acceleration for smoother movement
    if (mouse.GetCaptured(window)) {
        float sensitivity = 0.1f;
        mouseDelta *= sensitivity;

        if (enableCameraSmoothing) {
            yawVelocity = bl::DampExponential(yawVelocity, mouseDelta.x, cameraSmoothnessDampLambda, dt);
            pitchVelocity = bl::DampExponential(pitchVelocity, mouseDelta.y, cameraSmoothnessDampLambda, dt);
        } else {
            yawVelocity = mouseDelta.x;
            pitchVelocity = mouseDelta.y;
        }

        yaw += yawVelocity;
        pitch -= pitchVelocity;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        if (yaw < 0.0f)
            yaw += 360.0f;
        else if (yaw > 360.0f)
            yaw -= 360.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        SetRotation(direction);
    }

}

void FlyCamera3D::Draw(RenderData&)
{
    // ImGui::Begin("Camera Settings");
    // if (ImGui::TreeNode("Camera")) {
    //     ImGui::Checkbox("Enable Mouse Smoothing", &enableCameraSmoothing);
    //     ImGui::SetNextItemWidth(150.0f);
    //     ImGui::SliderFloat("Mouse Smoothness", &cameraSmoothnessDampLambda, 1.0f, 10.0f);

    //     ImGui::Dummy({0.0f, 10.0f});

    //     ImGui::Checkbox("Enable Movement Damping", &enableCameraMovementDamping);
    //     ImGui::SetNextItemWidth(150.0f);
    //     ImGui::SliderFloat("Acceleration", &cameraAcceleration, 0.1f, 20.0f);
    //     ImGui::SetNextItemWidth(150.0f);
    //     ImGui::SliderFloat("Max Speed", &maxCameraSpeed, 1.0f, 20.0f);
    //     ImGui::SetNextItemWidth(150.0f);
    //     ImGui::SliderFloat("Movement Damping", &cameraMovementDampLambda, 1.0f, 10.0f);

    //     ImGui::Separator();
    //     ImGui::Text("Position: %.2f, %.2f, %.2f m", position.x, position.y, position.z);
    //     ImGui::Text("Velocity: %.2f, %.2f, %.2f m/s", cameraVelocity.x, cameraVelocity.y, cameraVelocity.z);
    //     ImGui::Text("Rotation: %.2f, %.2f deg", yaw, pitch);
    //     ImGui::Text("Speed: %.2f m/s", glm::length(cameraVelocity));

    //     ImGui::TreePop();
    // }
    // ImGui::End();
}

void FlyCamera3D::SetSpeed(float speed)
{
    _speed = speed;
}

float FlyCamera3D::GetSpeed()
{
    return _speed;
}

void FlyCamera3D::SetSmoothness(float smoothness)
{
    _smooth = smoothness;
}

float FlyCamera3D::GetSmoothness()
{
    return _smooth;
}

void FlyCamera3D::RegisterClass(ClassDB& db)
{
    db.RegisterClass("FlyCamera3D", "Camera3D", &FlyCamera3D::Create);
    db.RegisterProperty("FlyCamera3D", std::make_unique<TProperty<FlyCamera3D, float>>(db, "speed", PropertyFlags::Editor, &FlyCamera3D::SetSpeed, &FlyCamera3D::GetSpeed));
    db.RegisterProperty("FlyCamera3D", std::make_unique<TProperty<FlyCamera3D, float>>(db, "smoothness", PropertyFlags::Editor, &FlyCamera3D::SetSmoothness, &FlyCamera3D::GetSmoothness));
}

}