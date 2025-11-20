#pragma once

namespace bl {

template <typename T>
struct TExtent3D {
    T width;
    T height;
    T depth;
};

template <typename T>
struct TExtent2D {
    T width;
    T height;

    TExtent3D<T> To3D()
    {
        return TExtent3D<T> { width, height, 1 };
    }
};

template <typename T>
struct TOffset2D {
    T x;
    T y;
};

using Extent2D = TExtent2D<uint32_t>;
using Extent3D = TExtent3D<uint32_t>;
using Offset2D = TExtent2D<int32_t>;

} // namespace bl