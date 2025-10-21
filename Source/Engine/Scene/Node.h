#pragma once

#include "Precompiled.h"

namespace bl
{

class Engine;

class Node : public std::enable_shared_from_this<Node>
{
    std::string _name;
    std::weak_ptr<Node> _parent;
    std::vector<std::shared_ptr<Node>> _children;
    Engine* _engine;

public:
    Node(Engine* engine);
    virtual ~Node();

    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate(float delta);
    virtual void Draw();
    virtual Node* Clone();

    Engine* GetEngine();
    void SetName(const std::string& name);
    std::string GetName() const;
    void SetParent(std::shared_ptr<Node> parent);
    std::shared_ptr<Node> GetParent() const;
    const std::vector<std::shared_ptr<Node>>& GetChildren() const;
    void AddChild(std::shared_ptr<Node> child);
    void RemoveChild(std::shared_ptr<Node> child);
    void ClearChildren();
};

} // namespace bl
