#pragma once

#include "Precompiled.h"
#include "Core/Json.h"
#include "Resource.h"

using 

namespace bl
{

class ResourceBuilder
{
    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& additionalData) = 0;
};

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void RegisterBuilder(std::vector<std::string> types, std::shared_ptr<ResourceBuilder> builder); /** @brief Registers a builder object to create resource references. */
    void LoadFromManifest(const std::filesystem::path& manifest);
    template<typename T> ResourceRef<T> Load(const std::string& name); /** @brief Loads any resource that isn't currently loaded into memory, just returns it if it already exists. */
    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */

private:
    std::multimap<std::string, std::shared_ptr<ResourceBuilder>> _builders; /** @brief These builders build the resources inside the engine, some builders can build more than one type of resource hence a multimap. */
    std::unordered_map<std::string, std::unique_ptr<Resource>> _resources;
};

} // namespace bl

/* How a resource manifest file works:

    It's a typical JSON file with data for each resource.

    Each resource itself has some data:
        type - string
        path - string       <- used as name in the resource manager 

    {
        "resources": [
            {
                "type": "sound"
                "path": "/some/path.wav"
            },
            {
                "type": "model",
                "path": "assets/models/fox.gltf"
            }
            {
                "type": "shader",
                "path": "assets/shaders/default_unlit.vert"
            }
        ]
    }

 */