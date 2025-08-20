#pragma once

#include "Core/Object.h"

class Node : public Object {
    OBJECT_CLASS(Node, Object);

    std::vector<Node*> _children;

};