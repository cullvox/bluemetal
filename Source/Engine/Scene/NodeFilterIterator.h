#pragma once

#include <span>
#include <stack>
#include <functional>

#include "Node.h"
#include "NodeFilter.h"

namespace bl {

using NodeFilterFunc = std::function<bool(Node&)>;

// @brief Hierarchical node filter for realtime node operations.
// Useful for quickly searching for nodes, filtering, and render culling.
// This is a pre-order search.
class NodeFilterIterator
{
    std::vector<NodeFilterFunc> _filters;
    std::stack<Node*> _stack;

    bool PassesFilters(Node* node);

public:
    NodeFilterIterator(Node* tree, std::span<NodeFilterFunc> filters);
    ~NodeFilterIterator();

    bool HasNext();
    Node* Next();
};

}