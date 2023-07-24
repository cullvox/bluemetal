#pragma once

namespace bl
{

typedef uint32_t 

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    template<typename TResource>
    std::shared_ptr<TResource> createResource();

private:
    std::unordered_map<std::string, std::unique_ptr<Resource>> m_resources;
}

} // namespace bl