#include "Node3D.h"

namespace bl {

Node3D::Node3D(Engine* engine)
    : Node(engine)
    , _transform(1.0f)
    , _position(0.0f, 0.0f, 0.0f)
    , _rotation(1.0f, 0.0f, 0.0f, 0.0f) // Identity quaternion
    , _scale(1.0f, 1.0f, 1.0f)
{
    UpdateTransform();
}

Node3D::~Node3D() = default;

void Node3D::Update(float deltaTime)
{
    Node::Update(deltaTime);
}

void Node3D::PhysicsUpdate(float delta)
{
    Node::PhysicsUpdate(delta);
}

void Node3D::Draw(VulkanRenderData& rd)
{
    Node::Draw(rd);
}

void Node3D::UpdateTransform()
{
    _transform = glm::mat4(1.0f);
    _transform = glm::translate(_transform, _position);
    _transform *= glm::mat4_cast(_rotation);
    _transform = glm::scale(_transform, _scale);

    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        _transform = parent->GetWorldTransform() * _transform;
    }

    for (const auto& child : GetChildren()) {
        if (auto child3D = std::dynamic_pointer_cast<Node3D>(child)) {
            child3D->UpdateTransform();
        }
    }
}

void Node3D::SetPosition(const glm::vec3& position)
{
    _position = position;
}

void Node3D::SetWorldPosition(const glm::vec3& position)
{
    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        glm::mat4 parentWorldTransform = parent->GetWorldTransform();
        glm::mat4 parentInverse = glm::inverse(parentWorldTransform);
        glm::vec4 localPos = parentInverse * glm::vec4(position, 1.0f);
        _position = glm::vec3(localPos);
    } else {
        _position = position;
    }
}

void Node3D::SetRotation(const glm::vec3& eulerAngles)
{
    _rotation = glm::quat(eulerAngles);
}

void Node3D::SetRotation(const glm::quat& rotation)
{
    _rotation = rotation;
}

void Node3D::SetWorldRotation(const glm::vec3& eulerAngles)
{
    SetWorldRotation(glm::quat(eulerAngles));
}

void Node3D::SetWorldRotation(const glm::quat& rotation)
{
    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        glm::quat parentWorldRotation = parent->GetWorldRotationQuat();
        glm::quat localRotation = glm::inverse(parentWorldRotation) * rotation;
        _rotation = localRotation;
    } else {
        _rotation = rotation;
    }
}

void Node3D::SetScale(const glm::vec3& scale)
{
    _scale = scale;
}

void Node3D::SetWorldScale(const glm::vec3& scale)
{
    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        glm::vec3 parentWorldScale = parent->GetWorldScale();
        _scale = scale / parentWorldScale;
    } else {
        _scale = scale;
    }
}

glm::vec3 Node3D::GetPosition() const
{
    return _position;
}

glm::vec3 Node3D::GetWorldPosition() const
{
    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        return glm::vec3(parent->GetWorldTransform() * glm::vec4(_position, 1.0f));
    } else {
        return _position;
    }
}

glm::vec3 Node3D::GetRotation() const
{
    return glm::eulerAngles(_rotation);
}

glm::quat Node3D::GetRotationQuat() const
{
    return _rotation;
}

glm::vec3 Node3D::GetWorldRotation() const
{
    return glm::eulerAngles(GetWorldRotationQuat());
}

glm::quat Node3D::GetWorldRotationQuat() const
{
    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        return parent->GetWorldRotationQuat() * _rotation;
    } else {
        return _rotation;
    }
}

glm::vec3 Node3D::GetScale() const
{
    return _scale;
}

glm::vec3 Node3D::GetWorldScale() const
{
    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        return parent->GetWorldScale() * _scale;
    } else {
        return _scale;
    }
}

glm::mat4 Node3D::GetTransform() const
{
    return _transform;
}

glm::mat4 Node3D::GetWorldTransform() const
{
    if (auto parent = std::dynamic_pointer_cast<Node3D>(GetParent())) {
        return parent->GetWorldTransform() * _transform;
    } else {
        return _transform;
    }
}

} // namespace bl
