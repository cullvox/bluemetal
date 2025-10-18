#pragma once

#include "Resource.h"

class SceneTree : public Resource
{
public:
    SceneTree(ResourceSystem* resourceSystem, WorldSystem* world, const std::filesystem::path& path);
    ~SceneTree();
};