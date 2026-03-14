#pragma once

#include "Node.h"

#include "Math/Math.h"

namespace bl {

/**
 * @class Node3D
 * @brief Represents a 3D node in the scene graph.
 */
class Node3D : public Node {
    glm::mat4 _matrix;
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
    glm::mat4 _worldMatrix;
    glm::vec3 _worldPosition;
    bool _isDirty;

    void SetDirty();
    void UpdateMatrix();
public:

    Node3D(Engine& engine);
    Node3D(const Node3D&);
    virtual ~Node3D();

    virtual Node3D* Clone();
    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate();
    virtual void Draw(RenderData& rd);

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& eulerAngleDegrees);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    void SetWorldPosition(const glm::vec3& position);
    void SetWorldRotation(const glm::quat& rotation);
    void SetWorldRotation(const glm::vec3& eulerAngleDegrees);
    void SetWorldScale(const glm::vec3& scale);

    const glm::vec3& GetPosition() const;
    glm::vec3 GetRotation() const;
    const glm::quat& GetRotationQuat() const;
    const glm::vec3& GetScale() const;
    const glm::mat4& GetMatrix();
    glm::vec3 GetWorldPosition();
    glm::vec3 GetWorldRotation();
    glm::quat GetWorldRotationQuat();
    glm::vec3 GetWorldScale();
    const glm::mat4& GetWorldMatrix();

};

} // namespace bl
