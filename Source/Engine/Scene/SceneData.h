#pragma once

namespace bl
{

class SceneData
{
    struct NodeData
    {
        std::string name;
        std::string classname;
        std::vector<std::pair<std::string, Variant>> properties;
    };

    struct ResourceData
    {
        
    };

    std::vector<NodeData> _nodes;
    std::vector<ResourceData> _resources;
};

}