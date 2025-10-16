#include "Node.h"

Node::Node() = default;
Node::~Node() = default;

void Node::Update(float deltaTime)
{
    (void)deltaTime;
}

void Node::PhysicsUpdate(float delta)
{
    (void)delta;
}

void Node::Draw()
{
}

void Node::SetName(const std::string& name)
{
    _name = name;
}

std::string Node::GetName() const
{
    return _name;
}

void Node::SetParent(std::shared_ptr<Node> parent)
{
    // Remove from current parent if exists.
    if (auto currentParent = _parent.lock()) {
        currentParent->RemoveChild(shared_from_this());
    }

    _parent = parent;

    if (parent) {
        parent->AddChild(shared_from_this());
    }
}

std::shared_ptr<Node> Node::GetParent() const
{
    return _parent.lock();
}

const std::vector<std::shared_ptr<Node>>& Node::GetChildren() const
{
    return _children;
}

void Node::AddChild(std::shared_ptr<Node> child)
{
    if (child) {
        // Avoid adding the same child multiple times.
        if (std::find(_children.begin(), _children.end(), child) == _children.end()) {
            _children.push_back(child);
            child->_parent = shared_from_this();
        }
    }
}

void Node::RemoveChild(std::shared_ptr<Node> child)
{
    auto it = std::find(_children.begin(), _children.end(), child);
    if (it != _children.end()) {
        (*it)->_parent.reset();
        _children.erase(it);
    }
}

void Node::ClearChildren()
{
    for (auto& child : _children) {
        if (child) {
            child->_parent.reset();
        }
    }
    _children.clear();
}
