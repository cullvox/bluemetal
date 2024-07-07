#pragma once

namespace bl {

class VoxelLimits {
public:
    constexpr static inline int ChunkSize = 16;
    constexpr static inline int ChunkSizeSquared = ChunkSize * ChunkSize;
    constexpr static inline int ChunkSizeCubed = ChunkSizeSquared * ChunkSize;
};

} // namespace bl