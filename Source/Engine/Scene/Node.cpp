#include "Node.h"
#include "Core/Print.h"

namespace bl {

Node::Node(Engine& engine)
    : Object(engine)
    , _parent(nullptr)
{
}

Node::Node(const Node& node)
    : Object(node)
    , _name(node._name)
    , _parent(nullptr) // Parent is not copied
{
    // Deep copy children
    for (const auto& child : node._children) {
        AddChild(child->Clone());
    }
}

Node::~Node() = default;

void Node::Update(float dt)
{
    // Update children
    for (auto& node : _children) {
        node->Update(dt);
    }
}

void Node::PhysicsUpdate()
{
    // Update children
    for (auto& node : _children) {
        node->PhysicsUpdate();
    }
}

void Node::Ready()
{
}

void Node::Draw(RenderData& rd)
{
    for (auto& node : _children) {
        node->Draw(rd);
    }
}

bool Node::SetName(const std::string& name)
{
    if (!_parent) {
        _name = name;
        return true;
    }

    // Rename in parent's children map.
    if (_parent->_childrenMap.find(name) != _parent->_childrenMap.end()) {
        Print::Warn("A sibling node with the name '{}' already exists in parent node '{}'.", name, _parent->GetName());
        return false;
    }

    auto it = _parent->_childrenMap.find(_name);
    if (it != _parent->_childrenMap.end()) {
        auto nodePtr = std::move(it->second);
        _parent->_childrenMap.erase(it);
        _parent->_childrenMap.insert({ name, std::move(nodePtr) });
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
    auto it = _childrenMap.find(name);
    if (it != _childrenMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<Node*> Node::GetChildren() const
{
    std::vector<Node*> out;
    std::transform(_children.begin(), _children.end(), std::back_inserter(out), [](const auto& uniqueNode){
        return uniqueNode.get();
    });
    return out;
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
    if (_childrenMap.find(child->GetName()) == _childrenMap.end()) {
        _childrenMap.insert({ child->GetName(), childPtr.get() });
        _children.push_back(std::move(childPtr));
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
    if (_childrenMap.find(child->GetName()) == _childrenMap.end()) {
        child->_parent = this;
        _childrenMap.insert({ child->GetName(), child.get() });
        _children.push_back(std::move(child));
    } else {
        Print::Warn("A child with the name '{}' already exists in node '{}'.", child->GetName(), this->GetName());
    }
}

std::unique_ptr<Node> Node::UnlinkChild(const std::string& child)
{
    auto it = _childrenMap.find(child);
    if (it == _childrenMap.end()) {
        Print::Warn("No child with the name '{}' exists in node '{}'.", child, this->GetName());
        return nullptr;
    }

    auto childPtr = it->second;
    childPtr->_parent = nullptr;
    _childrenMap.erase(it);

    auto vecIt = std::find_if(_children.begin(), _children.end(), [childPtr](auto& uniqueNode){
        return uniqueNode.get() == childPtr;
    });

    if (vecIt == _children.end()) {
        throw std::runtime_error("vector and map out of sync!");
    }

    auto uniqueNode = std::move(*vecIt);
    _children.erase(vecIt);

    return std::move(uniqueNode);
}

void Node::DeleteChild(const std::string& child)
{
    auto it = _childrenMap.find(child);
    if (it == _childrenMap.end()) {
        Print::Warn("No child with the name '{}' exists in node '{}'.", child, this->GetName());
        return;
    }

    auto childPtr = it->second;

    auto vecIt = std::find_if(_children.begin(), _children.end(), [childPtr](auto& uniqueNode){
        return uniqueNode.get() == childPtr;
    });

    childPtr->_parent = nullptr;

    _childrenMap.erase(it);
    _children.erase(vecIt);
}

std::vector<std::unique_ptr<Node>>& Node::GetVecChildren()
{
    return _children;
}

} // namespace bl
