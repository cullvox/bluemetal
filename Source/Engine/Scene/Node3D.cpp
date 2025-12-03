#include "Node3D.h"

namespace bl {

Node3D::Node3D(Engine& engine)
    : Node(engine)
    , _transform(1.0f)
    , position(0.0f, 0.0f, 0.0f)
    , rotation(1.0f, 0.0f, 0.0f, 0.0f) // Identity quaternion
    , scale(1.0f, 1.0f, 1.0f)
{
    UpdateTransform();
}

Node3D::Node3D(const Node3D& node)
    : Node(node)
    , _transform(node._transform)
    , position(node.position)
    , rotation(node.rotation)
    , scale(node.scale)
{
}

Node3D::~Node3D() = default;

Node3D* Node3D::Clone()
{
    Node3D* node = new Node3D(GetEngine());
    node->SetName(GetName());

    for (auto child : GetChildren()) {
        node->AddChild(child->Clone());
    }

    node->position = position;
    node->rotation = rotation;
    node->scale = scale;
    node->UpdateTransform();

    return node;
}

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
    _transform = glm::translate(_transform, position);
    _transform *= glm::mat4_cast(rotation);
    _transform = glm::scale(_transform, scale);
}

void Node3D::SetPosition(const glm::vec3& pos)
{
    position = pos;
    UpdateTransform();
}

void Node3D::SetWorldPosition(const glm::vec3& pos)
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        glm::mat4 parentWorldTransform = parent->GetWorldTransform();
        glm::mat4 parentInverse = glm::inverse(parentWorldTransform);
        glm::vec4 localPos = parentInverse * glm::vec4(pos, 1.0f);
        position = glm::vec3(localPos);
    } else {
        position = position;
    }
}

void Node3D::SetRotation(const glm::vec3& eulerAngles)
{
    SetRotation(glm::quat(glm::radians(eulerAngles)));
    UpdateTransform();
}

void Node3D::SetRotation(const glm::quat& newRotation)
{
    rotation = newRotation;
    UpdateTransform();
}

void Node3D::SetWorldRotation(const glm::vec3& eulerAngles)
{
    SetWorldRotation(glm::quat(glm::radians(eulerAngles)));
}

void Node3D::SetWorldRotation(const glm::quat& newRotation)
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        glm::quat parentWorldRotation = parent->GetWorldRotationQuat();
        glm::quat localRotation = glm::inverse(parentWorldRotation) * newRotation;
        rotation = localRotation;
    } else {
        rotation = rotation;
    }
}

void Node3D::SetScale(const glm::vec3& newScale)
{
    scale = newScale;
    UpdateTransform();
}

glm::vec3 Node3D::GetPosition() const
{
    return position;
}

glm::vec3 Node3D::GetRotation() const
{
    return glm::degrees(glm::eulerAngles(rotation));
}

glm::quat Node3D::GetRotationQuat() const
{
    return rotation;
}

glm::vec3 Node3D::GetScale() const
{
    return scale;
}

glm::vec3 Node3D::GetWorldPosition() const
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        return glm::vec3(parent->GetWorldTransform() * glm::vec4(position, 1.0f));
    } else {
        return position;
    }
}

glm::vec3 Node3D::GetWorldRotation() const
{
    return glm::degrees(glm::eulerAngles(GetWorldRotationQuat()));
}


glm::quat Node3D::GetWorldRotationQuat() const
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        return parent->GetWorldRotationQuat() * rotation;
    } else {
        return rotation;
    }
}

glm::vec3 Node3D::GetWorldScale() const
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        return parent->GetWorldScale() * scale;
    } else {
        return scale;
    }
}

glm::mat4 Node3D::GetTransform()
{
    return _transform;
}

glm::mat4 Node3D::GetWorldTransform() const
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        return parent->GetWorldTransform() * _transform;
    } else {
        return _transform;
    }
}

} // namespace bl
