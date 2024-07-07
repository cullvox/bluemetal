#pragma once

#include "Extent.h"

namespace bl {

template<typename TExt, typename TOff>
struct TRect2D {
    TOffset2D<TOff> offset;
    TExtent2D<TExt> extent;
};

using Rect2D = TRect2D<uint32_t, int32_t>;

} // namespace bl
