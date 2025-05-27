#include "Renderer.h"
#include "Material.h"

namespace bl
{

Material::Material(ResourceManager* manager, const nlohmann::json& data, VulkanDevice* device, Renderer* renderer)
    : Resource(manager, data)
{

    auto path = data["path"].get<std::string>();
    std::ifstream materialFile(path);

    std::string renderPass;
    std::string vertexPath, fragmentPath;
    nlohmann::json json;
    VulkanPipelineStateInfo info;
    try
    {
        json = nlohmann::json::parse(materialFile);

        renderPass = json["renderPass"].get<std::string>();
        vertexPath = json["shaders"]["vertex"].get<std::string>();
        fragmentPath = json["shaders"]["fragment"].get<std::string>();
        info = json["state"];
    }
    catch (const std::exception& e)
    {
        Log::Error("Could not parse material json: {}", e.what());
    }

    VkRenderPass pass;
    uint32_t subpass;
    renderer->GetRenderPass(renderPass, pass, subpass);

    _material = std::make_unique<VulkanMaterial>(device, pass, subpass, info, renderer->GetSwapchainImageCount());
}

Material::~Material()
{

}

}