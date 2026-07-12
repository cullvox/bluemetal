#pragma once

#include "Resource.h"
#include "Scene/Node.h"

namespace bl 
{

/// A scene  
class Scene : public Resource 
{
    std::vector<std::string> _resources;
    nlohmann::json _heirarchyData;

public:
    Scene(Engine& engine); // Empty scene
    Scene(Engine& engine, const std::filesystem::path& path);
    ~Scene();

    const std::vector<std::string>& GetResourceManifest();
    std::unique_ptr<Node> Instantiate();
};

}