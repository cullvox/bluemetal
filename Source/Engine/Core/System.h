#pragma once

#include "Precompiled.h"

namespace bl
{

class Engine;
class ResourceSystem;
class Resource;

class System
{
    Engine& engine;
public:
    System(Engine& engine)
        : engine(engine)
    {
    }

    virtual ~System() = default;

    virtual std::unique_ptr<Resource> ConstructResource(ResourceSystem* resourceSystem, std::size_t typeHash, const std::filesystem::path& path) = 0;

    Engine& GetEngine() { return engine; }
};

} // namespace bl