#include "Node.h"

namespace bl
{

Node::Node(Engine* engine)
    : _engine(engine)
{
}

Node::~Node() = default;

void Node::Update(float)
{
}

void Node::PhysicsUpdate(float)
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
    return node;
}

Engine* Node::GetEngine()
{
    return _engine;
}

void Node::SetName(const std::string& name)
{
    _name = name;
    if (auto parent = _parent.lock()) {
        parent->_children.erase(_name);
        parent->_children[name] = shared_from_this();
    }
}

std::string Node::GetName() const
{
    return _name;
}

void Node::SetParent(std::shared_ptr<Node> parent)
{
    // Remove from current parent if exists.
    if (auto currentParent = _parent.lock()) {
        currentParent->RemoveChild(this->GetName());
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

std::shared_ptr<Node> Node::GetChild(const std::string& name) const
{
    auto it = _children.find(name);
    if (it != _children.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Node>> Node::GetChildren() const
{
    std::vector<std::shared_ptr<Node>> childrenList;
    for (auto& [name, child] : _children) {
        if (child) {
            childrenList.push_back(child);
        }
    }
    return childrenList;
}

void Node::AddChild(std::shared_ptr<Node> child)
{
    if (child) {
        // Avoid adding the same child multiple times.
        if (_children.find(child->GetName()) == _children.end()) {
            _children.insert({child->GetName(), child});
            child->_parent = shared_from_this();
        }
    }
}

void Node::RemoveChild(const std::string& child)
{
    auto it = _children.find(child);
    if (it != _children.end()) {
        (*it).second->_parent.reset();
        _children.erase(it);
    }
}

void Node::ClearChildren()
{
    for (auto& [name, child]: _children) {
        if (child) {
            child->_parent.reset();
        }
    }
    _children.clear();
}

} // namespace bl
