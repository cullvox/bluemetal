#pragma once

#include "NodeFilter.h"
#include "Math/Frustum.h"

namespace bl {

struct NodeFilterFrustumCull {
    bool operator()(Node& node);
};

} // namespace bl