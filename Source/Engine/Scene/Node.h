#pragma once

#include "Core/Object.h"
#include <Core/Variant.h>

namespace bl {

class Engine;
class RenderData;
class Node;

class Node : public Object {
    OBJECT_BOILER(Node, Object)

    std::string _name;
    Node* _parent;
    std::vector<std::unique_ptr<Node>> _children;
    std::unordered_map<std::string, Node*> _childrenMap;

public:
    Node(Engine& engine);
    Node(const Node&);
    virtual ~Node();

    virtual void Ready(); // Children are ready.
    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate();
    virtual void Draw(RenderData& rd);

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

    void Set(const std::string& name, const Variant& value);
    Variant Get(const std::string& name);

    void SetName(const std::string& name);
    const std::string& GetName();
    void SetParent(Node* parent);
    virtual Node* GetParent() const;
    Node* GetChild(const std::string& name) const;
    std::vector<Node*> GetChildren() const;
    virtual void AddChild(Node* child);
    virtual void AddChild(std::unique_ptr<Node> child);
    std::unique_ptr<Node> UnlinkChild(const std::string& child);
    void DeleteChild(const std::string& child);
    std::vector<std::unique_ptr<Node>>& GetVecChildren();

    static void RegisterClass(ClassDB& db);
};

} // namespace bl
