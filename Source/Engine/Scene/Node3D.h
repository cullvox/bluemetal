#pragma once

#include "Node.h"

#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/mat4x4.hpp"

/** 
 * @class Node3D
 * @brief Represents a 3D node in the scene graph.
 */
class Node3D : public Node
{

    glm::mat4 _transform;
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;

public:
    Node3D() = default;
    virtual ~Node3D() = default;

    virtual void Update(float deltaTime) {}
    virtual void PhysicsUpdate(float delta) {}
    virtual void Draw() {}

    void SetPosition(const glm::vec3& position);
    void SetWorldPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& eulerAngles);
    void SetRotation(const glm::quat& rotation);
    void SetWorldRotation(const glm::vec3& eulerAngles);
    void SetWorldRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    void SetWorldScale(const glm::vec3& scale);
    glm::vec3 GetPosition() const;
    glm::vec3 GetWorldPosition() const;
    glm::vec3 GetRotation() const;
    glm::quat GetRotationQuat() const;
    glm::vec3 GetWorldRotation() const;
    glm::quat GetWorldRotationQuat() const;
    glm::vec3 GetScale() const;
    glm::vec3 GetWorldScale() const;
    glm::mat4 GetTransform() const;
    glm::mat4 GetWorldTransform() const;

};
