#pragma once

#include "Node.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace bl {

/**
 * @class Node3D
 * @brief Represents a 3D node in the scene graph.
 */
class Node3D : public Node {
    glm::mat4 _transform;

public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    Node3D(Engine& engine);
    Node3D(const Node3D&);
    virtual ~Node3D();

    virtual Node3D* Clone();
    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate(float delta);
    virtual void Draw(VulkanRenderData& rd);

    void UpdateTransform();
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& eulerAngleDegrees);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    glm::vec3 GetPosition() const;
    glm::vec3 GetRotation() const;
    glm::quat GetRotationQuat() const;
    glm::vec3 GetScale() const;
    glm::mat4 GetTransform();
    void SetWorldPosition(const glm::vec3& position);
    void SetWorldRotation(const glm::quat& rotation);
    void SetWorldRotation(const glm::vec3& eulerAngleDegrees);
    void SetWorldScale(const glm::vec3& scale);
    glm::vec3 GetWorldPosition() const;
    glm::vec3 GetWorldRotation() const;
    glm::quat GetWorldRotationQuat() const;
    glm::vec3 GetWorldScale() const;
    glm::mat4 GetWorldTransform() const;
};

} // namespace bl
