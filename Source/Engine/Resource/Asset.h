#pragma once

#include <Core/Precompiled.hpp>

namespace bl {

class Asset {
public:
    Asset(std::filesystem::path path) = default;
    virtual ~Asset() = default;

    virtual void load() = 0;
    virtual void unload() = 0;
};

} // namespace bl