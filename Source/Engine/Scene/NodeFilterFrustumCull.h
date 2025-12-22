#pragma once

#include "NodeFilter.h"
#include "Math/Frustum.h"

namespace bl {

struct NodeFilterFrustumCull {
    Frustum frustum;

    bool operator()(Node& node);
};

} // namespace bl