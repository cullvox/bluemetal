
#include <cassert>
#include "NodeFilterIterator.h"

namespace bl {

NodeFilterIterator::NodeFilterIterator(Node* tree, std::span<NodeFilterFunc> filters)
{
    _filters.assign(filters.begin(), filters.end());

    if (tree)
        _stack.push(tree);
}

NodeFilterIterator::~NodeFilterIterator()
{
}

bool NodeFilterIterator::PassesFilters(Node* node)
{
    assert(node != nullptr);

    bool passesFilters = true;
    for (auto& filter : _filters) {
        if (!filter(*node)) {
            passesFilters = false;
        }
    }

    return passesFilters;
}

bool NodeFilterIterator::HasNext()
{
    return !_stack.empty();
}

Node* NodeFilterIterator::Next()
{
    Node* current = _stack.top();
    _stack.pop();

    const auto& children = current->GetVecChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto child = it->get();
        if (PassesFilters(child))
            _stack.push(child);
    }

    return current;
}

}
