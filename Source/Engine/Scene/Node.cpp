#include "Node.h"
#include "Core/Print.h"

namespace bl {

Node::Node(Engine& engine)
    : _engine(engine)
    , _parent(nullptr)
{
}

Node::Node(const Node& node)
    : _engine(node._engine)
    , _name(node._name)
    , _parent(nullptr) // Parent is not copied
{
    // Deep copy children
    for (const auto& [name, child] : node._children) {
        AddChild(child->Clone());
    }
}

Node::~Node() = default;

void Node::Update(float dt)
{
    // Update children
    for (auto& [name, child] : _children) {
        child->Update(dt);
    }
}

void Node::PhysicsUpdate(float)
{
}

void Node::Ready()
{
}

void Node::Draw(VulkanRenderData& rd)
{
    for (auto& [name, child] : _children) {
        child->Draw(rd);
    }
}

Node* Node::Clone()
{
    Node* node = new Node(_engine);
    node->SetName(_name);

    for (auto& [name, child] : _children) {
        node->AddChild(child->Clone());
    }

    return node;
}

Engine& Node::GetEngine()
{
    return _engine;
}

bool Node::SetName(const std::string& name)
{
    if (!_parent) {
        _name = name;
        return true;
    }

    // Rename in parent's children map.
    if (_parent->_children.find(name) != _parent->_children.end()) {
        Print::Warn("A sibling node with the name '{}' already exists in parent node '{}'.", name, _parent->GetName());
        return false;
    }

    auto it = _parent->_children.find(_name);
    if (it != _parent->_children.end()) {
        auto nodePtr = std::move(it->second);
        _parent->_children.erase(it);
        _parent->_children.insert({ name, std::move(nodePtr) });
    }

    _name = name;

    return true;
}

std::string Node::GetName() const
{
    return _name;
}

void Node::SetParent(Node* parent)
{
    // Remove from current parent if exists.
    if (_parent) {
        _parent->UnlinkChild(this->GetName());
    }

    if (parent) {
        parent->AddChild(this);
    }
}

Node* Node::GetParent() const
{
    return _parent;
}

Node* Node::GetChild(const std::string& name) const
{
    auto it = _children.find(name);
    if (it != _children.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<Node*> Node::GetChildren() const
{
    std::vector<Node*> childrenList;
    for (auto& [name, child] : _children) {
        if (child) {
            childrenList.push_back(child.get());
        }
    }
    return childrenList;
}

void Node::AddChild(Node* child)
{
    if (!child) {
        Print::Warn("Attempted to add null child.");
        return;
    }

    if (child == this) {
        Print::Warn("A node cannot be a child of itself.");
        return;
    }

    if (child->_parent == this) {
        Print::Warn("Node with name '{}' is already a child of node '{}'.", child->GetName(), this->GetName());
        return; // Already a child of this node.
    }

    if (child->GetName().empty()) {
        Print::Warn("A node with no name cannot be a child!");
        return;
    }

    std::unique_ptr<Node> childPtr(nullptr);
    if (child->_parent) {
        childPtr = child->_parent->UnlinkChild(child->GetName());
    } else {
        childPtr.reset(child); // Take ownership if no parent.
    }

    // Avoid adding the same child multiple times.
    if (_children.find(child->GetName()) == _children.end()) {
        _children.insert({ child->GetName(), std::move(childPtr) });
        child->_parent = this;
    } else {
        Print::Warn("A child with the name '{}' already exists in node '{}'.", child->GetName(), this->GetName());
    }
}

void Node::AddChild(std::unique_ptr<Node> child)
{
    if (!child) {
        Print::Warn("Attempted to add null child.");
        return;
    }

    if (child.get() == this) {
        Print::Warn("A node cannot be a child of itself.");
        return;
    }

    if (child->_parent == this) {
        Print::Warn("Node '{}' is already a child of node '{}'.", child->GetName(), this->GetName());
        return; // Already a child of this node.
    }

    if (child->_parent) {
        child = child->_parent->UnlinkChild(child->GetName());
    }

    // Avoid adding the same child multiple times.
    if (_children.find(child->GetName()) == _children.end()) {
        child->_parent = this;
        _children.insert({ child->GetName(), std::move(child) });
    } else {
        Print::Warn("A child with the name '{}' already exists in node '{}'.", child->GetName(), this->GetName());
    }
}

std::unique_ptr<Node> Node::UnlinkChild(const std::string& child)
{
    auto it = _children.find(child);
    if (it == _children.end()) {
        Print::Warn("No child with the name '{}' exists in node '{}'.", child, this->GetName());
        return nullptr;
    }

    auto childPtr = std::move(it->second);
    childPtr->_parent = nullptr;
    _children.erase(it);
    return childPtr;
}

void Node::DeleteChild(const std::string& child)
{
    auto it = _children.find(child);
    if (it == _children.end()) {
        Print::Warn("No child with the name '{}' exists in node '{}'.", child, this->GetName());
        return;
    }

    it->second->_parent = nullptr;
    _children.erase(it);
}

} // namespace bl
