#pragma once

#include "Resource.h"
#include "Node.h"

namespace bl {

class SceneTree {
    std::unique_ptr<Node> _root;

public:
    SceneTree();
    ~SceneTree();

    void UpdateTree();
};

}
