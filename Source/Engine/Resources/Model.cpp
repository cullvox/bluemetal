#include <tiny_gltf.h>

#include "Core/FileByte.h"
#include "Core/Print.h"
#include "Graphics/ModelFormat.h"
#include "Graphics/UniformData.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanSampler.h"
#include "Model.h"
#include "Texture2D.h"

namespace bl {

Model::Model(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());

    if (!warn.empty())
        Print::Warn("GLTF Load: {}", warn);

    if (!err.empty())
        Print::Error("GLTF Load: {}", err);

    if (!res)
        throw std::runtime_error("Could not load a model file!");

    // Load meshes
 
    for (auto& mesh : model.meshes) {

        for (auto& primitive : mesh.primitives) {
            auto& indexAccessor = model.accessors[primitive.indices];

            size_t verticesCount = model.accessors[primitive.attributes.begin()->second].count;
            std::vector<Vertex> vertices { verticesCount };
            std::vector<uint32_t> indices {};
            indices.resize(indexAccessor.count);

            size_t indicesWidth = 0;
            switch (indexAccessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    indicesWidth = 1;
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: 
                    indicesWidth = 2;
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    indicesWidth = 4;
                    break;
                default:
                    throw std::runtime_error("Invalid indices width!");
                    break;
            }

            auto& indexView = model.bufferViews[indexAccessor.bufferView];
            auto& indexBuffer = model.buffers[indexView.buffer];
            if (indicesWidth == 4) {
                memcpy(indices.data(), indexBuffer.data.data(), indexView.byteLength);
            } else {
                for (int i = 0; i < indexAccessor.count; i++) {
                    const size_t offset = i * indicesWidth;
                    std::memcpy(&indices[i], &indexBuffer.data[offset], indicesWidth);
                }
            }

            for (auto& attrib : primitive.attributes) {
                auto& attribAccessor = model.accessors[attrib.second];
                auto& bufferView = model.bufferViews[attribAccessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];

                if (attrib.first == "POSITION") {
                    if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || attribAccessor.type != TINYGLTF_TYPE_VEC3) {
                        throw std::runtime_error("Invalid position type.");
                    }
                    for (int i = 0; i < attribAccessor.count; i++) {
                        // sorta unsafe
                        std::memcpy(&vertices[i].position, &buffer.data[i * sizeof(glm::vec3)], sizeof(glm::vec3));
                    }
                }

                if (attrib.first == "NORMAL") {
                    if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || attribAccessor.type != TINYGLTF_TYPE_VEC3) {
                        throw std::runtime_error("Invalid normal type.");
                    }
                    for (int i = 0; i < attribAccessor.count; i++) {
                        // sorta unsafe
                        std::memcpy(&vertices[i].position, &buffer.data[i * sizeof(glm::vec3)], sizeof(glm::vec3));
                    }
                }

                if (attrib.first == "TEXCOORD_0") {
                    if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || attribAccessor.type != TINYGLTF_TYPE_VEC2) {
                        throw std::runtime_error("Invalid normal type.");
                    }
                    for (int i = 0; i < attribAccessor.count; i++) {
                        // sorta unsafe
                        std::memcpy(&vertices[i].position, &buffer.data[i * sizeof(glm::vec2)], sizeof(glm::vec2));
                    }
                }
            }

            std::unique_ptr<Mesh> m;
            m->UploadVertices<Vertex>(vertices);
            m->UploadIndices(indices);
        }
    }

    // Build out the scene tree for model loading.
    const auto& scene = model.scenes[model.defaultScene];

    _root = std::make_unique<Node3D>(system->GetEngine());
    for (int i : scene.nodes)
    {
        _root->
    }


}

Model::~Model()
{
}

void Model::Draw(VulkanRenderData&, VulkanMaterialInstance*)
{
    for (int i = 0; i < (int)_meshes.size(); i++) {
    }
}

}