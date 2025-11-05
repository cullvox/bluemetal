#pragma once

#include "Precompiled.h"
#include "Graphics/VulkanRenderData.h"

namespace bl
{

class Engine;

class Node : public std::enable_shared_from_this<Node>
{
    std::string _name;
    std::weak_ptr<Node> _parent;
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
    std::shared_ptr<Node> GetParent() const;
    Node* GetChild(std::string_view name);
    std::vector<Node*> GetChildren() const;
    void AddChild(std::unique_ptr<Node> child);
    void RemoveChild(std::string_view name);
    void ClearChildren();
};

} // namespace bl
