#include "Material.h"
#include "Engine/Engine.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanConversions.h"
#include "Graphics/VulkanShader.h"
#include "Shader.h"

#include "ResourceSystem.h"

namespace bl {

Material::Material(ResourceSystem& resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path)
    : MaterialInstance(resourceSystem, graphicsSystem)
    , _graphicsSystem(graphicsSystem)
    , _renderer(graphicsSystem->GetRenderer())
{
    std::ifstream materialFile { path };
    if (!materialFile.is_open()) {
        throw std::runtime_error("Could not open material JSON file.");
    }

    std::string vertexPath, fragmentPath;
    nlohmann::json json;
    VulkanPipelineStateInfo info;
    int32_t descriptorSetLocation = 1;

    try {
        json = nlohmann::json::parse(materialFile);

        std::string renderPass = json.value("renderPass", "geometry");

        if (renderPass == "geometry") {
            info.pass = RenderPassType::eGeometry;
        } else if (renderPass == "selection") {
            info.pass = RenderPassType::eSelection;
        } else {
            Print::Warn("Invalid render pass type! Using geometry.");
            info.pass = RenderPassType::eGeometry;
        }

        if (!(json.contains("shaders") && json["shaders"].is_object() && json["shaders"].contains("vertex") && json["shaders"].contains("fragment") && json["shaders"]["vertex"].is_string() && json["shaders"]["fragment"].is_string())) {
            Print::Error("A material must contain a vertex and fragment shader!");
            return;
        }

        vertexPath = json["shaders"]["vertex"];
        fragmentPath = json["shaders"]["fragment"];

        if (json.contains("vertexState") && json["vertexState"].is_object()) {
            nlohmann::json& state = json["vertexState"];

            auto vertex = state.value("vertex", "Default");
            if (vertex == "Default") {
                info.vertexState.inputBindings = Vertex::GetBindingDescriptions();
                info.vertexState.inputAttribs = Vertex::GetBindingAttributeDescriptions();
            } else if (vertex == "Skinned") {
                info.vertexState.inputBindings = VertexSkinned::GetBindingDescriptions();
                info.vertexState.inputAttribs = VertexSkinned::GetBindingAttributeDescriptions();
            } else if (vertex == "Debug") {
                info.vertexState.inputBindings = VertexDebug::GetBindingDescriptions();
                info.vertexState.inputAttribs = VertexDebug::GetBindingAttributeDescriptions();
            } else {
                Print::Warn("Invalid vertex type: \"{}\". Using default and hoping for the best.", vertex);
            }

            std::string topology = state.value("topology", "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST");
            info.vertexState.primitiveRestartEnable = state.value("primitiveRestartEnable", false);

            info.vertexState.topology = VulkanConversions::VkPrimitiveTopologyFromString(topology, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        }

        if (json.contains("descriptorSetLocation") && json["descriptorSetLocation"].is_number_integer()) {
            descriptorSetLocation = json.value("descriptorSetLocation", 1);
        }

        if (json.contains("rasterizerState") && json["rasterizerState"].is_object()) {
            nlohmann::json& state = json["rasterizerState"];
            info.rasterizerState.depthClampEnable = state.value("depthClampEnable", false);
            info.rasterizerState.rasterizerDiscardEnable = state.value("rasterizerDiscardEnable", false);
            std::string polygonMode = state.value("polygonMode", "VK_POLYGON_MODE_FILL");
            std::string cullMode = state.value("cullMode", "VK_CULL_MODE_BACK_BIT");
            std::string frontFace = state.value("frontFace", "VK_FRONT_FACE_COUNTER_CLOCKWISE");
            info.rasterizerState.depthBiasEnable = state.value("depthBiasEnable", false);
            info.rasterizerState.depthBiasConstantFactor = state.value("depthBiasConstantFactor", 0.0f);
            info.rasterizerState.depthBiasClamp = state.value("depthBiasClamp", 0.0f);
            info.rasterizerState.depthBiasSlopeFactor = state.value("depthBiasSlopeFactor", 0.0f);
            info.rasterizerState.lineWidth = state.value("lineWidth", 1.0f);

            info.rasterizerState.polygonMode = VulkanConversions::VkPolygonModeFromString(polygonMode, VK_POLYGON_MODE_FILL);
            info.rasterizerState.cullMode = VulkanConversions::VkCullModeFlagsFromString(cullMode, VK_CULL_MODE_BACK_BIT);
            info.rasterizerState.frontFace = VulkanConversions::VkFrontFaceFromString(frontFace, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        }

        if (json.contains("multisampleState") && json["multisampleState"].is_object()) {
            const nlohmann::json& state = json["multisampleState"];
            info.multisampleState.sampleShadingEnable = state.value("sampleShadingEnable", false);
            info.multisampleState.minSampleShading = state.value("minSampleShading", 0.2f);
            info.multisampleState.alphaToCoverageEnable = state.value("alphaToCoverageEnable", false);
            info.multisampleState.alphaToOneEnable = state.value("alphaToOneEnable", false);
        }

        if (json.contains("depthStencilState") && json["depthStencilState"].is_object()) {
            const nlohmann::json& state = json["depthStencilState"];

            info.depthStencilState.depthTestEnable = state.value("depthTestEnable", VK_TRUE);
            info.depthStencilState.depthWriteEnable = state.value("depthWriteEnable", VK_TRUE);
            std::string depthCompareOp = state.value("depthCompareOp", "VK_COMPARE_OP_LESS_OR_EQUAL");
            info.depthStencilState.depthBoundsTestEnable = state.value("depthBoundsTestEnable", VK_FALSE);
            info.depthStencilState.stencilTestEnable = state.value("stencilTestEnable", VK_FALSE);
            info.depthStencilState.minDepthBounds = state.value("minDepthBounds", 0.0f);
            info.depthStencilState.maxDepthBounds = state.value("maxDepthBounds", 1.0f);

            info.depthStencilState.depthCompareOp = VulkanConversions::VkCompareOpFromString(depthCompareOp);

            if (state.contains("front") && state["front"].is_object()) {
                const nlohmann::json& front = state["front"];

                std::string failOp = front.value("failOp", "VK_STENCIL_OP_KEEP");
                std::string passOp = front.value("passOp", "VK_STENCIL_OP_KEEP");
                std::string depthFailOp = front.value("depthFailOp", "VK_STENCIL_OP_KEEP");
                std::string compareOp = front.value("compareOp", "VK_COMPARE_OP_LESS_OR_EQUAL");
                info.depthStencilState.front.compareMask = front.value("compareMask", 0UL);
                info.depthStencilState.front.writeMask = front.value("writeMask", 0UL);
                info.depthStencilState.front.reference = front.value("reference", 0UL);

                info.depthStencilState.front.failOp = VulkanConversions::VkStencilOpFromString(failOp);
                info.depthStencilState.front.passOp = VulkanConversions::VkStencilOpFromString(passOp);
                info.depthStencilState.front.depthFailOp = VulkanConversions::VkStencilOpFromString(depthFailOp);
                info.depthStencilState.front.compareOp = VulkanConversions::VkCompareOpFromString(compareOp);
            }

            if (state.contains("back") && state["back"].is_object()) {
                const nlohmann::json& back = state["back"];

                std::string failOp = back.value("failOp", "VK_STENCIL_OP_KEEP");
                std::string passOp = back.value("passOp", "VK_STENCIL_OP_KEEP");
                std::string depthFailOp = back.value("depthFailOp", "VK_STENCIL_OP_KEEP");
                std::string compareOp = back.value("compareOp", "VK_COMPARE_OP_LESS_OR_EQUAL");
                info.depthStencilState.back.compareMask = back.value("compareMask", 0UL);
                info.depthStencilState.back.writeMask = back.value("writeMask", 0UL);
                info.depthStencilState.back.reference = back.value("reference", 0UL);

                info.depthStencilState.back.failOp = VulkanConversions::VkStencilOpFromString(failOp);
                info.depthStencilState.back.passOp = VulkanConversions::VkStencilOpFromString(passOp);
                info.depthStencilState.back.depthFailOp = VulkanConversions::VkStencilOpFromString(depthFailOp);
                info.depthStencilState.back.compareOp = VulkanConversions::VkCompareOpFromString(compareOp);
            }
        }

        if (json.contains("colorBlendState") && json["colorBlendState"].is_object()) {
            const nlohmann::json& state = json["colorBlendState"];

            info.colorBlendState.logicOpEnable = state.value("logicOpEnable", false);
            std::string logicOp = state.value("logicOp", "VK_LOGIC_OP_COPY");

            info.colorBlendState.logicOp = VulkanConversions::VkLogicOpFromString(logicOp);

            if (state.contains("attachments") && state["attachments"].is_array()) {

                info.colorBlendState.attachments.clear();
                for (const nlohmann::json& attachment : state["attachments"]) {
                    if (!attachment.is_object()) {
                        Print::Warn("Attachment must be an object.");
                        continue;
                    }

                    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
                    colorBlendAttachmentState.blendEnable = attachment.value("blendEnable", true);
                    std::string srcColorBlendFactor = attachment.value("srcColorBlendFactor", "VK_BLEND_FACTOR_SRC_ALPHA");
                    std::string dstColorBlendFactor = attachment.value("dstColorBlendFactor", "VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA");
                    std::string colorBlendOp = attachment.value("colorBlendOp", "VK_BLEND_OP_ADD");
                    std::string srcAlphaBlendFactor = attachment.value("srcAlphaBlendFactor", "VK_BLEND_FACTOR_ONE");
                    std::string dstAlphaBlendFactor = attachment.value("dstAlphaBlendFactor", "VK_BLEND_FACTOR_ZERO");
                    std::string alphaBlendOp = attachment.value("alphaBlendOp", "VK_BLEND_OP_ADD");
                    std::string colorWriteMask = attachment.value("colorWriteMask", "VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT");

                    colorBlendAttachmentState.srcColorBlendFactor = VulkanConversions::VkBlendFactorFromString(srcColorBlendFactor);
                    colorBlendAttachmentState.dstColorBlendFactor = VulkanConversions::VkBlendFactorFromString(dstColorBlendFactor);
                    colorBlendAttachmentState.colorBlendOp = VulkanConversions::VkBlendOpFromString(colorBlendOp);
                    colorBlendAttachmentState.srcAlphaBlendFactor = VulkanConversions::VkBlendFactorFromString(srcAlphaBlendFactor);
                    colorBlendAttachmentState.dstAlphaBlendFactor = VulkanConversions::VkBlendFactorFromString(dstAlphaBlendFactor);
                    colorBlendAttachmentState.alphaBlendOp = VulkanConversions::VkBlendOpFromString(alphaBlendOp);
                    colorBlendAttachmentState.colorWriteMask = VulkanConversions::VkColorComponentFlagsFromString(colorWriteMask);
                    info.colorBlendState.attachments.push_back(colorBlendAttachmentState);
                }
            }

            if (state.contains("blendConstants")) {
                if (!(state["blendConstants"].is_array() && state["blendConstants"].size() == 4)) {
                    Print::Warn("Blend constants must be an array of four numbers.");
                } else {
                    int i = 0;
                    for (const nlohmann::json& blendConstant : state["blendConstants"]) {

                        if (!blendConstant.is_number()) {
                            Print::Warn("Blend constant value must be a number.");
                            i++;
                            continue;
                        }

                        info.colorBlendState.blendConstants[i] = blendConstant.get<float>();
                        i++;
                    }
                }
            }
        }

        if (json.contains("dynamicStates") && json["dynamicStates"].is_array()) {
            const nlohmann::json& dynamicStates = json["dynamicStates"];
            for (const nlohmann::json& state : dynamicStates) {
                if (!state.is_string()) {
                    Print::Warn("Dynamic state must be a string!");
                    continue;
                }

                VkDynamicState dynamicState;
                if (!VulkanConversions::VkDynamicStateFromString(state.get<std::string>(), dynamicState)) {
                    Print::Warn("Invalid dynamic state input!");
                    continue;
                }

                info.dynamicStates.push_back(dynamicState);
            }
        }

        auto vertexShader = resourceSystem.Load<Shader>(vertexPath);
        auto fragmentShader = resourceSystem.Load<Shader>(fragmentPath);
        info.stages.shaders = std::vector<VulkanShader*> { vertexShader.lock()->Get(), fragmentShader.lock()->Get() };
    } catch (const nlohmann::json::exception& e) {
        Print::Error("Could not parse material JSON file. Error: {}", e.what());
        return;
    }

    _material = std::make_unique<VulkanMaterial>(graphicsSystem->GetDevice(), _renderer, info, descriptorSetLocation);
    _renderer->AddMaterial(_material.get());
}

Material::~Material()
{
    _renderer->RemoveMaterial(_material.get());
}

VulkanMaterialInstance* Material::GetInstance() const
{
    return _material.get();
}

std::shared_ptr<MaterialInstance> Material::CreateInstance()
{
    return std::make_shared<MaterialInstance>(GetResourceSystem(), _graphicsSystem, std::move(_material->CreateInstance()));
}

const VulkanPipeline* Material::GetVulkanPipeline()
{
    return _material->GetPipeline();
}

VulkanMaterial* Material::GetVulkanMaterial()
{
    return _material.get();
}

}