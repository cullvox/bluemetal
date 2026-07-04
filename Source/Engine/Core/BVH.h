#pragma once

#include "AABB.h"

namespace bl
{


/// @brief Bounded Volume Heirarchy
class BVH
{

    struct Node
    {
        AABB aabb;

        uint32_t start;
        uint32_t primitiveCount;
        uint32_t rightOffset;

        inline constexpr bool isLeaf() const noexcept { return rightOffset == 0; }
    };

    std::vector<Node> _nodes;


public:
    BVH() = default;
    ~BVH() = default;

    
};

} // namespace bl

