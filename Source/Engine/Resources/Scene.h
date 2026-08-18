#pragma once

#include "Resource.h"
#include "Scene/Node.h"

namespace bl 
{

/// A scene  
class Scene : public Resource 
{
    std::vector<std::string> _resources;
    nlohmann::json _data;

public:
    Scene(); // Empty scene
    Scene(const std::filesystem::path& path);
    ~Scene();

    /**
     * @brief Releases the scene data.
     */
    virtual void Release();

    /**
     * @brief Saves the packed node tree to the scene file.
     */
    virtual void Save();

    /**
     * @brief Packs the a node tree back into this resource.
     */
    void Pack(Node* root);

    /**
     * @brief Instantiates the node tree using the scene data from file.
     *
     * - Loads resources from file. (subresources are not being implemented yet)
     * - Builds out the node tree.
     *
     */
    std::unique_ptr<Node> Instantiate();

};

}