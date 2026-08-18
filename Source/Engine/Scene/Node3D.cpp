#include "Node3D.h"
#include "Core/ClassDB.h"
#include "Core/Reflection/Property.h"

namespace bl {

Node3D::Node3D()
    : Node()
    , _matrix(1.0f)
    , _position(0.0f, 0.0f, 0.0f)
    , _rotation(1.0f, 0.0f, 0.0f, 0.0f) // Identity quaternion
    , _scale(1.0f, 1.0f, 1.0f)
    , _worldMatrix(1.0f)
    , _worldPosition(0.0f)
    , _isDirty(true)
{
    UpdateMatrix();
}

Node3D::Node3D(const Node3D& node)
    : Node(node)
    , _matrix(node._matrix)
    , _position(node._position)
    , _rotation(node._rotation)
    , _scale(node._scale)
    , _worldMatrix(node._worldMatrix)
    , _isDirty(node._isDirty)
{
}

Node3D::~Node3D() = default;

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

void Node3D::SetWorldScale(glm::vec3 newScale)
{
    if (auto parent = dynamic_cast<Node3D*>(GetParent())) {
        UpdateMatrix();
        glm::vec3 parentWorldScale = parent->GetWorldScale();
        glm::vec3 localScale = (1.0f / parentWorldScale) * newScale;
        _scale = localScale;
    } else {
        _scale = newScale;
    }

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

void Node3D::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Node3D", "Node", &Node3D::Create);
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::vec3>>("position", PropertyFlags::Editor, &Node3D::SetPosition, &Node3D::GetPosition));
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::quat>>("rotation", PropertyFlags::Editor, &Node3D::SetRotation, &Node3D::GetRotation));
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::vec3>>("rotationEuler", PropertyFlags::Editor, &Node3D::SetRotationEuler, &Node3D::GetRotationEuler));
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::vec3>>("scale", PropertyFlags::Editor, &Node3D::SetScale, &Node3D::GetScale));
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::vec3>>("worldPosition", PropertyFlags::None, &Node3D::SetWorldPosition, &Node3D::GetWorldPosition));
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::quat>>("worldRotation", PropertyFlags::None, &Node3D::SetWorldRotation, &Node3D::GetWorldRotation));
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::vec3>>("worldRotationEuler", PropertyFlags::None, &Node3D::SetWorldRotationEuler, &Node3D::GetWorldRotationEuler));
    db->RegisterProperty("Node3D", std::make_unique<TProperty<Node3D, glm::vec3>>("worldScale", PropertyFlags::None, &Node3D::SetWorldScale, &Node3D::GetWorldScale));
}


} // namespace bl
