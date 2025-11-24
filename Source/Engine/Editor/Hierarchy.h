#pragma once

#include "Scene/Node.h"

namespace bl {

class EditorHierarchy {

public:
    EditorHierarchy(Node* root);
    ~EditorHierarchy();

    virtual void Draw();

};

} // namespace bl
