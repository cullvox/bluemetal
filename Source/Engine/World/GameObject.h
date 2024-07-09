#pragma once

#include "Precompiled.h"
#include "Component.h"

namespace bl
{

/// @brief Object that all game objects will inherit from.
///
/// Creates a heiarchical tree structure of game objects.
class GameObject
{
public:
    GameObject();
    ~GameObject();

    std::string GetName();
    std::vector<GameObject*> GetChildren();

private:
    std::string _name;
    std::vector<GameObject*> _children;
};

} // namespace bl