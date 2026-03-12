#include "CharacterBody3D.h"
#include "Engine/Engine.h"
#include "Window/Input.h"
#include "Scene/Orbit3D.h"

namespace bl {

CharacterBody3D::CharacterBody3D(Engine& engine)
    : PhysicsBody3D(engine)
{
    _orbiter = nullptr;
}

CharacterBody3D::~CharacterBody3D() = default;

void CharacterBody3D::Update(float dt)
{
    PhysicsBody3D::Update(dt);

    if (!_orbiter)
        _orbiter = GetChild("Orbiter")->As<Orbit3D>();

    auto input = GetEngine().GetInput();
    auto& keyboard = input->GetKeyboard();
    auto& mouse = input->GetMouse();

    glm::vec3 velocity = GetVelocity();

    if (keyboard.GetKeyDown(Scancode::W)) {
        // Move forward
        glm::vec3 forward = GetWorldRotationQuat() * glm::vec3(0.0f, 0.0f, 1.0f);
        velocity += forward * 5.0f * dt;
    }

    if (keyboard.GetKeyDown(Scancode::S)) {
        // Move backward
        glm::vec3 backward = GetWorldRotationQuat() * glm::vec3(0.0f, 0.0f, -1.0f);
        velocity += backward * 5.0f * dt;
    }

    if (keyboard.GetKeyDown(Scancode::A)) {
        // Move left
        glm::vec3 left = GetWorldRotationQuat() * glm::vec3(1.0f, 0.0f, 0.0f);
        velocity += left * 5.0f * dt;
    }

    if (keyboard.GetKeyDown(Scancode::D)) {
        // Move right
        glm::vec3 right = GetWorldRotationQuat() * glm::vec3(-1.0f, 0.0f, 0.0f);
        velocity += right * 5.0f * dt;
    }

    if (keyboard.GetKeyDown(Scancode::Space)) {
        // Jump
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        velocity += glm::vec3(0, 20, 0) * dt;
        //ApplyImpulse(up * 4.0f);
    }

    // Normalize horizontal movement to prevent faster diagonal movement
    glm::vec3 horizontalVelocity = glm::vec3(velocity.x, 0.0f, velocity.z);
    if (glm::length(horizontalVelocity) > 5.0f) {
        horizontalVelocity = glm::normalize(horizontalVelocity) * 5.0f;
    }
    velocity.x = horizontalVelocity.x;
    velocity.z = horizontalVelocity.z;

    SetVelocity(velocity);

    // Rotate the player to input.
    if (mouse.GetCaptured(GetEngine().GetWindow()))
    {
        glm::vec2 delta = mouse.GetMouseDelta();

        _orbiter->RotateAzimuth(glm::radians(delta.x));
        _orbiter->RotatePolar(glm::radians(delta.y));
    }
    
}

}