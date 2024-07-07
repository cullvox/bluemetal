#pragma once

#include "Chunk.h"

namespace bl {

class GeneratorInterface {
public:
    virtual ~GeneratorInterface() = default;

    virtual void Generate(Chunk& chunk);
};

} // namespace bl