#pragma once

namespace bl
{

class VoxelLimits 
{
public:
    constexpr static inline int ChunkSize = 16;
    constexpr static inline int ChunkSizeSquared = 256;
    constexpr static inline int ChunkSizeCubed = 4096;
};

} // namespace bl