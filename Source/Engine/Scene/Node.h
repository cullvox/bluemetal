#pragma once

#include "Precompiled.h"

class Node : public std::enable_shared_from_this<Node>
{
    std::string _name;
    std::weak_ptr<Node> _parent;
    std::vector<std::shared_ptr<Node>> _children;

public:
    Node();
    virtual ~Node();

    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate(float delta);
    virtual void Draw();

    void SetName(const std::string& name);
    std::string GetName() const;
    void SetParent(std::shared_ptr<Node> parent);
    std::shared_ptr<Node> GetParent() const;
    const std::vector<std::shared_ptr<Node>>& GetChildren() const;
    void AddChild(std::shared_ptr<Node> child);
    void RemoveChild(std::shared_ptr<Node> child);
    void ClearChildren();
};
