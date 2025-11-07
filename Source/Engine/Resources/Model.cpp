

#include "Core/FileByte.h"
#include "Core/Print.h"
#include "Graphics/ModelFormat.h"
#include "Graphics/UniformData.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanSampler.h"
#include "Model.h"
#include "Texture2D.h"

#include "Scene/MeshInstance3D.h"

#include <glm/gtx/matrix_decompose.hpp>

namespace bl {

Model::Model(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool res = false;
    if (path.extension() == ".glb") {
        res = loader.LoadBinaryFromFile(&model, &err, &warn, path.string());
    } else {
        res = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());
    }

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
                memcpy(indices.data(), indexBuffer.data.data() + indexView.byteOffset, 4 * indexAccessor.count);
            } else {
                for (int i = 0; i < indexAccessor.count; i++) {
                    std::memcpy(&indices[i], indexBuffer.data.data() + indexView.byteOffset + (i * indicesWidth), indicesWidth);
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
                        size_t offset = bufferView.byteOffset + (i * sizeof(glm::vec3));
                        std::memcpy(&vertices[i].position, &buffer.data[offset], sizeof(glm::vec3));
                    }
                }

                if (attrib.first == "NORMAL") {
                    if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || attribAccessor.type != TINYGLTF_TYPE_VEC3) {
                        throw std::runtime_error("Invalid normal type.");
                    }
                    for (int i = 0; i < attribAccessor.count; i++) {
                        // sorta unsafe
                        size_t offset = i * sizeof(glm::vec3) + bufferView.byteOffset;
                        std::memcpy(&vertices[i].normal, &buffer.data[offset], sizeof(glm::vec3));
                    }
                }

                if (attrib.first == "TEXCOORD_0") {
                    if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || attribAccessor.type != TINYGLTF_TYPE_VEC2) {
                        throw std::runtime_error("Invalid normal type.");
                    }
                    for (int i = 0; i < attribAccessor.count; i++) {
                        // sorta unsafe
                        size_t offset = i * sizeof(glm::vec2) + bufferView.byteOffset;
                        std::memcpy(&vertices[i].texCoords, &buffer.data[offset], sizeof(glm::vec2));
                    }
                }
            }

            auto m = std::make_shared<Mesh>(resourceSystem, system, "");
            AddSubResource(m);
            _meshes.push_back(m);
            m->UploadVertices<Vertex>(vertices);
            m->UploadIndices(indices);
        }
    }

    // Build out the scene tree for model loading.
    const auto& scene = model.scenes[model.defaultScene];

    _root = std::make_unique<Node3D>(&system->GetEngine());

    for (int i : scene.nodes)
    {
        _root->AddChild(LoadNode(model, model.nodes[i], system));
    }
}

Model::~Model()
{
}

std::shared_ptr<Node3D> Model::LoadNode(const tinygltf::Model& model, const tinygltf::Node& node, GraphicsSystem* system)
{
    std::shared_ptr<Node3D> newNode{nullptr};
    if (node.mesh < 0) {
        newNode = std::make_unique<Node3D>(&system->GetEngine());
    } else {
        auto meshNode = std::make_unique<MeshInstance3D>(&system->GetEngine());
        meshNode->SetMesh(_meshes[node.mesh]);
        // meshNode->SetMaterial(_materials[...]);
        newNode = std::move(meshNode);
    }

    newNode->SetName(node.name);

    // Load transform
    if (node.matrix.size() == 16) {
        glm::mat4 transform;
        std::memcpy(&transform, node.matrix.data(), sizeof(glm::mat4));
        // Decompose matrix
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(transform, scale, rotation, translation, skew, perspective);
        newNode->SetPosition(translation);
        newNode->SetRotation(rotation);
        newNode->SetScale(scale);
    } else {
        if (node.translation.size() == 3) {
            glm::vec3 translation;
            std::memcpy(&translation, node.translation.data(), sizeof(glm::vec3));
            newNode->SetPosition(translation);
        }
        if (node.rotation.size() == 4) {
            glm::quat rotation;
            std::memcpy(&rotation, node.rotation.data(), sizeof(glm::quat));
            newNode->SetRotation(rotation);
        }
        if (node.scale.size() == 3) {
            glm::vec3 scale;
            std::memcpy(&scale, node.scale.data(), sizeof(glm::vec3));
            newNode->SetScale(scale);
        }
    }

    for (int i : node.children) {
        newNode->AddChild(LoadNode(model, model.nodes[i], system));
    }
    return newNode;
}

std::shared_ptr<Node3D> Model::GetTree()
{
    return _root;
}

}