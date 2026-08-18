#pragma once

#include "Node.h"

#include "Math/Math.h"

namespace bl {

/**
 * @class Node3D
 * @brief Represents a 3D node in the scene graph.
 */
class Node3D : public Node {
    OBJECT_BOILER(Node3D, Node)

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
    Node3D();
    Node3D(const Node3D&);
    virtual ~Node3D();

    virtual void Update(float deltaTime) override;
    virtual void PhysicsUpdate() override;
    virtual void Draw(RenderData& rd) override;

    void SetPosition(glm::vec3 position);
    void SetRotationEuler(glm::vec3 eulerDegrees);
    void SetRotation(glm::quat rotation);
    void SetScale(glm::vec3 scale);
    void SetWorldPosition(glm::vec3 position);
    void SetWorldRotation(glm::quat rotation);
    void SetWorldRotationEuler(glm::vec3 eulerDegrees);
    void SetWorldScale(glm::vec3 scale);

    glm::vec3 GetPosition();
    glm::vec3 GetRotationEuler();
    glm::quat GetRotation();
    glm::vec3 GetScale();
    const glm::mat4& GetMatrix();
    glm::vec3 GetWorldPosition();
    glm::vec3 GetWorldRotationEuler();
    glm::quat GetWorldRotation();
    glm::vec3 GetWorldScale();
    const glm::mat4& GetWorldMatrix();

    static void RegisterClass();
};

} // namespace bl
