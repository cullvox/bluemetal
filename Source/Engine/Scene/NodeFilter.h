#pragma once

namespace bl {

class Node;

struct NodeFilter
{
    bool operator()(Node&);
};

} // namespace bl