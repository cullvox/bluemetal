#pragma once

namespace bl {

template<typename T>
struct TExtent2D {
    T width;
    T height;
};

template<typename T>
struct TOffset2D {
    T x;
    T y;
};

using Extent2D = TExtent2D<uint32_t>;
using Offset2D = TExtent2D<int32_t>;

} // namespace bl