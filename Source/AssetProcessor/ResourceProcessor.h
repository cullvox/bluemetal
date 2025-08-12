#pragma once

#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_set>

struct ResourceFile {
    std::string type;
    std::string relativePath;
    std::filesystem::path absolutePath;
    std::filesystem::path bakedPath; // Empty if no baking took place.
    nlohmann::json properties;
};

struct ProcessorState {
    std::vector<ResourceFile> resources;
    std::unordered_set<std::string> resourceChecker;
    std::filesystem::path manifestPath;
    std::filesystem::path outputPath;
    std::filesystem::path materialOutputPath;
};

class ResourceProcessor
{
public:
    ResourceProcessor() = default;
    virtual ~ResourceProcessor() = default;

    virtual bool Process(ProcessorState& state, ResourceFile& resource) = 0;
};