#pragma once

#include "Graphics/VulkanRenderData.h"
#include "Precompiled.h"

namespace bl {

class Engine;

class Node : public std::enable_shared_from_this<Node> {
    std::string _name;
    std::shared_ptr<Node> _parent;
    std::unordered_map<std::string, std::shared_ptr<Node>> _children;
    Engine* _engine;

public:
    Node(Engine* engine);
    virtual ~Node();

    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate(float delta);
    virtual void Draw(VulkanRenderData& rd);
    virtual Node* Clone();

    Engine* GetEngine();
    void SetName(const std::string& name);
    std::string GetName() const;
    void SetParent(std::shared_ptr<Node> parent);
    virtual std::shared_ptr<Node> GetParent() const;
    std::shared_ptr<Node> GetChild(const std::string& name) const;
    std::vector<std::shared_ptr<Node>> GetChildren() const;
    virtual void AddChild(std::shared_ptr<Node> child);
    void RemoveChild(const std::string& child);
    void ClearChildren();
};

} // namespace bl
