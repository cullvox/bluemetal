#pragma once

#include "Precompiled.h"
#include "Component.h"

namespace bl
{

class GameObject
{
public:
    GameObject();
    ~GameObject();

    std::string GetName();
    template<typename T> bool HasComponent() const;
    template<typename T> void AddComponent();
    template<typename T> void RemoveComponent();
    template<typename T> T* GetComponent() const;
    GameObject* GetChild(int index) const;
    std::vector<GameObject*> GetChildren();

private:
    std::string _name;
    std::unordered_map<size_t, std::unique_ptr<Component>> _components;
    std::vector<GameObject*> _children;
};

template<typename T>
bool GameObject::HasComponent() const
{
    auto type = typeid(T);
    return _components.find(type.hash_code()) != _components.end();
}

template<typename T>
void GameObject::AddComponent()
{
    // Components must derive from Component and be default constructable.
    static_assert(std::is_base_of_v<Component, T> == true && "T must be derived from Component!"); 
    static_assert(std::is_default_constructable_v<T> == true && "T Component must be default constructable!");

    // Ensure that the component wasn't already added.
    auto type = typeid(T);
    if (HasComponent<T>())
    {
        blWarning("Component \"{}\" already exists on \"{}\", not adding another!", type.name(), _name);
        return;
    }

    _components.emplace(type.hash_code(), std::make_unique<T>());
}

template<typename T>
void GameObject::RemoveComponent()
{
    auto type = typeid(T);
    if (!HasComponent<T>())
    {
        blWarning("Trying to remove component \"{}\" from \"{}\" that does not exist!", type.name(), _name);
        return;
    }
 
    _components.erase(type.hash_code());
}

} // namespace bl