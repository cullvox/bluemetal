#pragma once

#include <stdexcept>
#include <memory>
#include <string>
#include <unordered_map>

#include "Graphics/VulkanRenderData.h"

namespace bl {

class Engine;

class Node {
    std::string _name;
    Node* _parent;
    std::unordered_map<std::string, std::unique_ptr<Node>> _children;
    Engine& _engine;

public:
    Node(Engine& engine);
    Node(const Node&);
    virtual ~Node();

    virtual void Ready(); // Children are ready.
    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate(float delta);
    virtual void Draw(VulkanRenderData& rd);
    virtual Node* Clone(); // Creates a non-owning deep copy of this node and its children.

    template <typename T>
    T* As()
    {
        T* value = dynamic_cast<T*>(this);
        if (value) {
            return value;
        } else {
            throw std::runtime_error("Failed to cast Node to desired type!");
        }
    }

    Engine& GetEngine();
    bool SetName(const std::string& name);
    std::string GetName() const;
    void SetParent(Node* parent);
    virtual Node* GetParent() const;
    Node* GetChild(const std::string& name) const;
    std::vector<Node*> GetChildren() const;
    virtual void AddChild(Node* child);
    virtual void AddChild(std::unique_ptr<Node> child);
    std::unique_ptr<Node> UnlinkChild(const std::string& child);
    void DeleteChild(const std::string& child);
};

} // namespace bl
