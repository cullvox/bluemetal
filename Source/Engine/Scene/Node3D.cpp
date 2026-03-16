#include "Node3D.h"

#include "Core/Reflection/Property.h"

namespace bl {

Node3D::Node3D(Engine& engine)
    : Node(engine)
    , _matrix(1.0f)
    , _worldMatrix(1.0f)
    , _position(0.0f, 0.0f, 0.0f)
    , _rotation(1.0f, 0.0f, 0.0f, 0.0f) // Identity quaternion
    , _scale(1.0f, 1.0f, 1.0f)
    , _isDirty(true)
{
    UpdateMatrix();
}

Node3D::Node3D(const Node3D& node)
    : Node(node)
    , _matrix(node._matrix)
    , _worldMatrix(node._worldMatrix)
    , _position(node._position)
    , _rotation(node._rotation)
    , _scale(node._scale)
    , _isDirty(node._isDirty)
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

    node->SetPosition(_position);
    node->SetRotation(_rotation);
    node->SetScale(_scale);
    node->UpdateMatrix();

    return node;
}

void Node3D::Update(float deltaTime)
{
    Node::Update(deltaTime);
}

void Node3D::PhysicsUpdate()
{
    Node::PhysicsUpdate();
}

void Node3D::Draw(RenderData& rd)
{
    Node::Draw(rd);
}

void Node3D::UpdateMatrix()
{
    //if (!_isDirty) {
    //    return;
    //}

    glm::mat4 T = glm::translate(glm::mat4(1.0f), _position);
    glm::mat4 R = glm::mat4_cast(_rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), _scale);

    _matrix = T * R * S;

    // Update world matrix if there's a parent
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        _worldMatrix = parent->GetWorldMatrix() * _matrix;
    } else {
        _worldMatrix = _matrix;
    }

    // Update world position
    _worldPosition = glm::vec3(_worldMatrix[3]);


    _isDirty = false;
}

void Node3D::SetDirty()
{
    _isDirty = true;
    for (auto& child : GetVecChildren()) {
        if (Node3D* child3D = dynamic_cast<Node3D*>(child.get())) {
            child3D->SetDirty();
        }
    }
}

void Node3D::SetPosition(glm::vec3 pos)
{
    _position = pos;
    SetDirty();
}

void Node3D::SetWorldPosition(glm::vec3 pos)
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        const glm::mat4& parentWorldTransform = parent->GetWorldMatrix();
        glm::mat4 parentInverse = glm::inverse(parentWorldTransform);
        glm::vec4 localPos = parentInverse * glm::vec4(pos, 1.0f);
        _position = glm::vec3(localPos);
    } else {
        _position = pos;
    }

    SetDirty();
}

void Node3D::SetRotationEuler(glm::vec3 eulerDegrees)
{
    SetRotation(glm::quat(glm::radians(eulerDegrees)));
}

void Node3D::SetRotation(glm::quat newRotation)
{
    _rotation = newRotation;
    SetDirty();
}

void Node3D::SetWorldRotationEuler(glm::vec3 eulerDegrees)
{
    SetWorldRotation(glm::quat(glm::radians(eulerDegrees)));
}

void Node3D::SetWorldRotation(glm::quat newRotation)
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        UpdateMatrix();
        glm::quat parentWorldRotation = parent->GetWorldRotation();
        glm::quat localRotation = glm::inverse(parentWorldRotation) * newRotation;
        _rotation = localRotation;
    } else {
        _rotation = newRotation;
    }

    SetDirty();
}

void Node3D::SetScale(glm::vec3 newScale)
{
    _scale = newScale;
    SetDirty();
}

glm::vec3 Node3D::GetPosition()
{
    return _position;
}

glm::vec3 Node3D::GetRotationEuler()
{
    return glm::degrees(glm::eulerAngles(_rotation));
}

glm::quat Node3D::GetRotation()
{
    return _rotation;
}

glm::vec3 Node3D::GetScale()
{
    return _scale;
}

glm::vec3 Node3D::GetWorldPosition()
{
    UpdateMatrix();
    return _worldPosition;
}

glm::vec3 Node3D::GetWorldRotationEuler()
{
    return glm::degrees(glm::eulerAngles(GetWorldRotation()));
}


glm::quat Node3D::GetWorldRotation()
{
    UpdateMatrix();
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        return parent->GetWorldRotation() * _rotation;
    } else {
        return _rotation;
    }
}

glm::vec3 Node3D::GetWorldScale()
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        return parent->GetWorldScale() * _scale;
    } else {
        return _scale;
    }
}

const glm::mat4& Node3D::GetMatrix()
{
    UpdateMatrix();
    return _matrix;
}

const glm::mat4& Node3D::GetWorldMatrix()
{
    UpdateMatrix();
    return _worldMatrix;
}

void Node3D::Register()
{
    TProperty("position", &Node3D::SetPosition, &Node3D::GetPosition);
    TProperty("rotation", &Node3D::SetRotation, &Node3D::GetRotation);
    TProperty("rotationEuler", &Node3D::SetRotationEuler, &Node3D::GetRotationEuler);
    TProperty("scale", &Node3D::SetScale, &Node3D::GetScale);

    TProperty("worldPosition", &Node3D::SetWorldPosition, &Node3D::GetWorldPosition);
    TProperty("worldRotation", &Node3D::SetWorldRotation, &Node3D::GetWorldRotation);
    TProperty("worldRotationEuler", &Node3D::SetWorldRotationEuler, &Node3D::GetWorldRotationEuler);
    TProperty("worldScale", &Node3D::SetWorldScale, &Node3D::GetWorldScale);
}


} // namespace bl
