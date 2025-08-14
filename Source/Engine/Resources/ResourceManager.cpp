#include "ResourceManager.h"
#include "Core/Print.h"

namespace bl {

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::BuildResourcesFromManifest(const std::string& manifestPath)
{
    // Open the JSON manifest file.
    std::ifstream file(manifestPath, std::ios::binary | std::ios::in);
    if (file.bad()) {
        Print::Error("Could not open the resource manifest file! ({})", manifestPath);
        return false;
    }

    // Parse the json file.
    nlohmann::json root;
    try {
        root = nlohmann::json::parse(file);
    } catch (const std::exception& e) {
        Print::Error("Manifest parsing error: {}", e.what());
        return false;
    }

    // Iterate through all resources and obtain their data.
    int entryNumber = 0;
    for (const auto& data : root) {
        auto path = data["relativePath"].get<std::string>();
        auto type = data["type"].get<std::string>();
        auto bakedPath = data["bakedPath"].get<std::string>();
        auto importData = data["importData"];

        if (path.empty() || type.empty()) {
            Print::Error("Invalid resource entry data \"{}\" in manifest \"{}\"!", entryNumber, manifestPath);
            entryNumber++;
            continue;
        }

        entryNumber++;

        // Finish building resource and move onto the next.
        Object* resourceObject = ObjectClasses::CreateObject(type);
        Resource* resource = dynamic_cast<Resource*>(resourceObject);

        if (resourceObject == nullptr || resource == nullptr) {
            Print::Error("Invalid resource type or object type \"{}\"!", type);
            Print::Error("Could not build resource \"{}\" in manifest \"{}\"!", path, manifestPath);
            return false; // We got big fish to fry with this error.
        }

        resource->_manager = this;
        resource->_path = path;
        resource->_source = ResourceSource::eFile;
        resource->_isLoaded = false;
        resource->_importData = importData;

        _resources[path] = std::move(std::unique_ptr<Resource>(resource));
    }

    return true;
}

void ResourceManager::UnloadUnreferenced()
{
}

bool ResourceManager::Unlist(const std::string& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Print::Error("Could not find resource!");
        return false;
    }

    Resource* resource = (*it).second.get();
    resource->InvalidateReferences();

    Print::Info("Unlisting resource \"{}\".", path);

    _resources.erase(path);
}

bool ResourceManager::Load(const std::string& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Print::Error("Could not find resource!");
        return false;
    }

    return it->second.get()->Load();
}

bool ResourceManager::LoadGroup(const std::vector<std::string>& paths)
{
    bool noFail = true;
    for (const std::string& path : paths) {
        auto it = _resources.find(path);
        if (it == _resources.end()) {
            Print::Error("Could not find resource!");
            noFail = false;
            continue;
        }

        if (!it->second.get()->Load())
            noFail = false;
    }

    return noFail;
}

bool ResourceManager::GetResourcePack(Resource* resource, std::ifstream& outFile, std::size_t& outByteSize)
{
    // Pedantically ensure that this resource exists and is what it says it is.
    outByteSize = 0;
    auto it = _resources.find(resource->_path);
    if (it == _resources.end() || (*it).second.get() != resource) {
        Print::Error("Could not find resource!");
        return false;
    }

    // Also ensure that this resource is stored as a binary.
    if (resource->_source != ResourceSource::eBinary)
        return false;

    // Open the file stream that this resource belongs to.
    std::ifstream pack(_packPaths[resource->_packIndex], std::ios::in | std::ios::binary);
    if (!pack.good())
        return false;

    pack.seekg(resource->_packByteOffset);

    if (!pack.good())
        return false;

    outFile = std::move(pack);
    outByteSize = resource->_packByteSize;

    return true;
}

} // namespace bl