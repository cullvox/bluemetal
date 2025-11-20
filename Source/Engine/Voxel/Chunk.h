#pragma once

#include "Precompiled.h"
#include "Voxel.h"
#include "VoxelLimits.h"

namespace bl {

class Chunk {
public:
    Chunk();
    ~Chunk();

    void Set(glm::ivec3 localPosition, Voxel voxel);

private:
    std::array<Voxel, VoxelLimits::ChunkSizeCubed> _voxels;
};

} // namespace bl