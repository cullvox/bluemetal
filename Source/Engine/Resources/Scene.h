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

    /**
     * @brief Packs the a node tree back into this resource.
     */
    void Pack(std::unique_ptr<Node> tree);

    /**
     * @brief Instantiates the node tree using the scene data from file.
     *
     * - Loads resources from file. (subresources are not being implemented yet)
     * - Builds out the node tree.
     *
     */
    std::unique_ptr<Node> Instantiate();


    /**
     * @brief Saves the packed node tree to the scene file.
     */
    virtual void Save();

};

}