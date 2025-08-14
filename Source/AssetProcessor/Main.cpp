
// The asset processing pipeline happens automagically before the game(s) are compiled.
//
// We convert the many asset types possibly used in game to file types that are faster
// and more accessible to the engine.
//
// Here's a basic table of how assets are converted.
//
// Texture -> QOI (Quite Ok Image)
// Static Model -> A custom binary of packed vertices and images directly after
// Sound -> FLAC
// Shader (GLSL) -> SPIR-V using glslc

#include <assimp/material.h>
#include <filesystem>
#include <fstream>
#include <regex>

#include <argparse/argparse.hpp>

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Graphics/ModelFormat.h"
#include "qoixx.hpp"

#include "Core/FileByte.h"
#include "Core/Print.h"
#include "Graphics/Vertex.h"
#include "Graphics/stb_image.h"

#include "ResourceProcessor.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ONLY_SERIALIZE(ResourceFile, type, relativePath, bakedPath)

std::filesystem::path GetBakedParentPath(ProcessorState& state, ResourceFile& resource);
bool ProcessShader(ProcessorState& state, ResourceFile& resource);
bool ProcessTexture(ProcessorState& state, ResourceFile& resource);
bool ProcessAudio(ProcessorState& state, ResourceFile& resource);
bool ProcessModel(ProcessorState& state, ResourceFile& resource);
bool ProcessGeneric(ProcessorState& state, ResourceFile& resource);

int main(int argc, const char** argv)
{
    argparse::ArgumentParser parser { "AssetProcessor", "0.1" };
    parser
        .add_argument("-m", "--manifest")
        .help("The resource manifest containing all project resources.")
        .required();
    parser
        .add_argument("-o", "--bakedPath")
        .help("The path where all baked resources end up when processed.")
        .required();
    parser
        .add_argument("-mo", "--materialOutputPath")
        .help("Exported model materials can be exported to a separate material folder.")
        .default_value("");
    parser
        .add_argument("-mt", "--materialTextureOutputPath")
        .help("Exports a models textures into a specific folder.");
    parser
        .add_argument("-v", "--verbose")
        .help("Turns on verbose logging mode.")
        .default_value(false);

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& e) {
        bl::Print::Error("{}, {}", e.what(), parser.help().str());
        std::exit(EXIT_FAILURE);
    }

    ProcessorState state;

    // Process the manifest file.
    state.manifestPath = parser.get<std::string>("manifest");
    state.outputPath = parser.get<std::string>("bakedPath");
    state.materialOutputPath = parser.get<std::string>("materialOutputPath");

    if (parser.get<bool>("verbose")) {
        bl::Print::EnableVerboseLogging(true);
        bl::Print::Verbose("Enabling verbose logging.");
    }

    std::filesystem::path manifestRoot = state.manifestPath.parent_path();

    try {
        std::ifstream manifestFile(state.manifestPath);
        auto manifestJson = nlohmann::json::parse(manifestFile);
        manifestFile.close();

        auto objectArray = manifestJson["resources"];

        for (auto object : objectArray) {
            ResourceFile resource;
            resource.type = object["type"].get<std::string>();
            resource.relativePath = object["relativePath"].get<std::string>();
            resource.absolutePath = manifestRoot / resource.relativePath;
            resource.bakedPath.clear();

            // Ensure that the path doesn't exist yet.
            if (state.resourceChecker.find(resource.relativePath) != state.resourceChecker.end()) {
                bl::Print::Error("{}: Already exists, two resources cannot have the same path! Skipping...", resource.relativePath);
                continue;
            }

            state.resources.push_back(resource);
            state.resourceChecker.emplace(resource.relativePath);
        }

    } catch (...) {
        bl::Print::Error("Could not parse the manifest file!");
        exit(EXIT_FAILURE);
    }

    for (auto& resource : state.resources) {
        // Ensure that the resource actually exists.
        if (!std::filesystem::exists(resource.absolutePath) || !std::filesystem::is_regular_file(resource.absolutePath)) {
            bl::Print::Error("Resource does not exist or is not a file: {}", resource.absolutePath);
            continue;
        }

        bl::Print::Verbose("Beginning processing of: {}", resource.relativePath);
        bool status = false;

        if (resource.type == "Shader") {
            status = ProcessShader(state, resource);
        } else if (resource.type == "Texture") {
            status = ProcessTexture(state, resource);
        } else if (resource.type == "Audio") {
            status = ProcessGeneric(state, resource);
        } else if (resource.type == "Model") {
            status = ProcessModel(state, resource);
        } else if (resource.type == "Material") {
            status = ProcessGeneric(state, resource);
        }

        if (status)
            bl::Print::Info("{}: Processed successfully.", resource.relativePath);
        else
            bl::Print::Error("{}: Could not be processed.", resource.relativePath);
    }

    nlohmann::json manifestJson;
    for (const auto& resource : state.resources) {
        nlohmann::json resourceJson;
        resourceJson = resource;
        resourceJson["properties"] = resource.properties;

        manifestJson["resources"].push_back(resourceJson);

        // if (resource.bakedPath.empty())
        //     manifestJson["resources"].erase("bakedPath");
    }

    std::ofstream manifestFile(state.manifestPath);
    manifestFile << std::setw(4) << manifestJson << std::endl;
    manifestFile.close();

    return EXIT_SUCCESS;
}

std::filesystem::path GetBakedParentPath(ProcessorState& state, ResourceFile& resource)
{
    auto exportedPath = state.outputPath / resource.relativePath;
    std::filesystem::create_directories(exportedPath.parent_path());

    auto filename = exportedPath.filename();
    auto relativeExportedPath = std::filesystem::path(resource.relativePath).parent_path();
    relativeExportedPath /= filename.string();

    auto relativePath = std::filesystem::relative(state.outputPath.parent_path(), state.manifestPath.parent_path().parent_path());
    auto newPath = relativePath / relativeExportedPath;

    return newPath;
}

bool ProcessShader(ProcessorState& state, ResourceFile& resource)
{
    // Build the final absolutePath, with proper 'spv' extension.
    auto bakedPath = GetBakedParentPath(state, resource);

    // Maybe increase this checks complexity down the line!
    std::string shaderType = bakedPath.extension() == ".vert" ? "vertex" : "fragment";

    bakedPath.replace_extension(bakedPath.extension().string() + ".spv");

    // Run the glslc shader compilation command.
    std::string cmd = fmt::format("glslc {} -o {}", resource.absolutePath.string(), bakedPath.string());
    if (std::system(cmd.c_str()) != EXIT_SUCCESS) {
        bl::Print::Error("{}: Could not compile shader resource.", resource.relativePath);
        bl::Print::Warn("This asset will not be added to the engine manifest.");
        bl::Print::Warn("Please ensure that you have the Vulkan SDK Installed.");
        return false;
    }

    // Export the final resource into the engine manifest.
    resource.bakedPath = bakedPath;
    resource.properties["shaderType"] = shaderType;
    return true;
}

bool ProcessTexture(ProcessorState& state, ResourceFile& resource)
{
    // Build the final absolutePath, with proper qoi extension.
    auto bakedPath = GetBakedParentPath(state, resource);
    bakedPath.replace_extension(".qoi");

    if (resource.absolutePath.extension() == ".qoi") {
        bl::Print::Info("{}: Already in QOI format, not baking.", bakedPath.string());
        auto relativePath = std::filesystem::relative(resource.absolutePath.parent_path(), state.manifestPath.parent_path().parent_path());
        resource.bakedPath = relativePath / resource.absolutePath.filename();
        return true;
    }

    if (resource.absolutePath.extension() != ".png" && resource.absolutePath.extension() != ".jpg" && resource.absolutePath.extension() != ".jpeg") {
        bl::Print::Error("{}: Invalid texture file type, please convert it manually.", resource.relativePath);
        return false;
    }

    int x = 0, y = 0, channels = 0;
    auto data = stbi_load(resource.absolutePath.string().c_str(), &x, &y, &channels, 4);

    if (data == nullptr) {
        bl::Print::Error("{}: Could not load this texture.", resource.absolutePath.string());
        return false;
    }

    qoixx::qoi::desc desc;
    desc.width = x;
    desc.height = y;
    desc.channels = 4;
    desc.colorspace = qoixx::qoi::colorspace::linear; // We assume every texture is linear, it's easier to do math on!

    auto out = qoixx::qoi::encode<std::vector<char>, stbi_uc>(data, x * y * 4, desc);

    std::ofstream outFile(bakedPath, std::ios::out | std::ios::binary);
    outFile.write(out.data(), out.size());
    outFile.close();

    resource.bakedPath = bakedPath;
    return true;
}

bool ProcessGeneric(ProcessorState& state, ResourceFile& resource)
{
    auto relativePath = std::filesystem::relative(resource.absolutePath.parent_path(), state.manifestPath.parent_path().parent_path());
    resource.bakedPath = relativePath / resource.absolutePath.filename();
    return true;
}
