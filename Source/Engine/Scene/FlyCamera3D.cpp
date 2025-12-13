#include "FlyCamera3D.h"
#include "Engine/Engine.h"
#include "Window/Input.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"
#include "Window/Window.h"
#include "Graphics/VulkanWindow.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Renderer.h"

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
    auto window = GetEngine().GetGraphics().GetWindow();
    auto renderer = GetEngine().GetGraphics().GetRenderer();

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
        UpdateTransform();
    } else {
        // Treat acceleration as direct position change
        position += acceleration * dt;
        UpdateTransform();
    }

    // Apply friction to camera velocity
    if (enableCameraMovementDamping && !cameraMoved) {
        cameraVelocity = bl::DampExponential(cameraVelocity, glm::zero<glm::vec3>(), cameraMovementDampLambda, dt);
    } else if (!cameraMoved) {
        cameraVelocity = glm::zero<glm::vec3>();
    }

    auto mousePos = mouse.GetMousePosition();
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

    view = glm::lookAt(position, position + cameraFront, cameraUp);

    bl::Extent2D extent = window->GetExtent();
    glm::vec2 extentf = glm::vec2 { (float)extent.width, (float)extent.height };
    glm::mat4 projection = glm::perspective(glm::radians(70.0f), extentf.x / extentf.y, 0.1f, 1000.0f);
    projection[1][1] *= -1; // Invert the projection for Vulkan y (0, 1)

    renderer->SetView(view);
    renderer->SetProjection(projection);
}

void FlyCamera3D::Draw(VulkanRenderData& rd)
{
    ImGui::Begin("Camera Settings");
    if (ImGui::TreeNode("Camera")) {
        ImGui::Checkbox("Enable Mouse Smoothing", &enableCameraSmoothing);
        ImGui::SetNextItemWidth(150.0f);
        ImGui::SliderFloat("Mouse Smoothness", &cameraSmoothnessDampLambda, 1.0f, 10.0f);

        ImGui::Dummy({0.0f, 10.0f});

        ImGui::Checkbox("Enable Movement Damping", &enableCameraMovementDamping);
        ImGui::SetNextItemWidth(150.0f);
        ImGui::SliderFloat("Acceleration", &cameraAcceleration, 0.1f, 20.0f);
        ImGui::SetNextItemWidth(150.0f);
        ImGui::SliderFloat("Max Speed", &maxCameraSpeed, 1.0f, 20.0f);
        ImGui::SetNextItemWidth(150.0f);
        ImGui::SliderFloat("Movement Damping", &cameraMovementDampLambda, 1.0f, 10.0f);

        ImGui::Separator();
        ImGui::Text("Position: %.2f, %.2f, %.2f m", position.x, position.y, position.z);
        ImGui::Text("Velocity: %.2f, %.2f, %.2f m/s", cameraVelocity.x, cameraVelocity.y, cameraVelocity.z);
        ImGui::Text("Rotation: %.2f, %.2f deg", yaw, pitch);
        ImGui::Text("Speed: %.2f m/s", glm::length(cameraVelocity));

        ImGui::TreePop();
    }
    ImGui::End();
}

void FlyCamera3D::SetSpeed(float speed)
{
}

void FlyCamera3D::SetSmoothness(float smoothness)
{
}

}