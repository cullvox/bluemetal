#pragma once

#include "Core/Object.h"

namespace bl {

class ResourceSystem;
class Resource;

class System : public Object {
protected:
    friend class Engine;

    System() = default;
    virtual ~System() = default;

public:
    virtual std::shared_ptr<Resource> ConstructResource(std::size_t typeHash, const std::filesystem::path& path);
};

} // namespace bl