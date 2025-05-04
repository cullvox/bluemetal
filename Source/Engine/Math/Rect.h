#pragma once

#include "Extent.h"

namespace bl {

template<typename TExt, typename TOff>
struct TRect2D {
    TOffset2D<TOff> offset;
    TExtent2D<TExt> extent;

    TOffset2D<TExt> Midpoint() 
    {
        return {offset.x + extent.x / 2, offset.y + extent.y / 2};
    }
};

using Rect2D = TRect2D<uint32_t, int32_t>;
using Rect2Df = TRect2D<float, float>;

} // namespace bl
