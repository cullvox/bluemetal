#pragma once

#include <Core/Precompiled.hpp>

class blAsset {
public:
    blAsset() = default;
    virtual ~blAsset() = default;

    virtual void load() = 0;
    virtual void unload() = 0;
};