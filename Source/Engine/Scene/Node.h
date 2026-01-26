#pragma once

#include <stdexcept>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <Core/Variant.h>

namespace bl {

class Engine;
class RenderData;
class Node;

class PropertyBase {
public:
    virtual void Set(Node* node, const Variant& value) = 0;
    virtual Variant Get(Node* node) = 0;
};

template<typename N, typename T>
class Property : public PropertyBase {
    T N::* _ptr;
    std::string_view _name;

public:
    Property(std::string_view name, N::T* value)
        : _name(name)
        , _ptr(value)
    {
    }

    virtual void Set(Node* node, const Variant& value)
    {
        if (auto n = dynamic_cast<N>(node))
            n->*_ptr = value;
    }

    virtual Variant Get(Node* node)
    {
        if (auto n = dynamic_cast<N>(node))
            return n->_ptr;
    }
};

class Node {
    std::string _name;
    Node* _parent;
    std::vector<std::unique_ptr<Node>> _children;
    std::unordered_map<std::string, Node*> _childrenMap;
    Engine& _engine;
    static std::vector<PropertyBase> _properties;
    static std::unordered_map<std::string_view, int> _propertyNameToIndex;

protected:
    friend class NodeFilterIterator;
    const std::vector<std::unique_ptr<Node>>& GetVecChildren();

public:
    Node(Engine& engine);
    Node(const Node&);
    virtual ~Node();

    virtual void Ready(); // Children are ready.
    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate(float delta);
    virtual void Draw(RenderData& rd);
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

    void Set(const std::string& name, const Variant& value);
    Variant Get(const std::string& name);
    void Set(int propertyIndex, const Variant& value);
    Variant Get(int propertyIndex);

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
