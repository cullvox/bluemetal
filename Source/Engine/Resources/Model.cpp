#include <tiny_gltf.h>

#include "Core/FileByte.h"
#include "Core/Print.h"

#include "Engine/Engine.h"

#include "Graphics/GraphicsSystem.h"
#include "Graphics/UniformData.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanSampler.h"

#include "Resources/Material.h"
#include "Resources/MaterialInstance.h"
#include "Resources/Model.h"
#include "Resources/ResourceSystem.h"
#include "Resources/Texture2D.h"

#include "Scene/MeshInstance3D.h"
#include <glm/gtc/type_ptr.hpp>

namespace bl {

std::unique_ptr<Node3D> Model::LoadNode(const tinygltf::Model& model, const tinygltf::Node& node)
{
    std::unique_ptr<Node3D> newNode { nullptr };
    if (node.mesh < 0) {
        newNode = std::make_unique<Node3D>();
    } else {
        auto meshNode = std::make_unique<MeshInstance3D>();
        auto& primitive = model.meshes[node.mesh].primitives[0];

        if (primitive.material == -1) {
            meshNode->SetMaterial(_materials[0]);
        } else {
            meshNode->SetMaterial(_materials[primitive.material]);
        }

        meshNode->SetMesh(_meshes[node.mesh]);

        newNode = std::move(meshNode);
    }

    newNode->SetName(node.name);

    // Load transform
    if (node.matrix.size() == 16) {
        glm::mat4 transform = glm::make_mat4x4(node.matrix.data());

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
        newNode->AddChild(std::move(LoadNode(model, model.nodes[i])));
    }
    return newNode;
}

Model::Model()
{
}

Model::Model(const std::filesystem::path& path)
    : Resource(path)
{
}

Model::Model(const Model& copy)
    : Resource(copy)
{
}

Model::~Model()
{
}

Node3D* Model::GetTree()
{
    return _root.get();
}

const std::vector<Ref<Mesh>>& Model::GetMeshes() const
{
    return _meshes;
}

void Model::Load()
{
    auto resourceSystem = ResourceSystem::Get();

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    const auto& json = GetJson();
    auto path = std::filesystem::path(json["path"].get<std::string>());

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

    // Load images
    _textures.resize(model.images.size());
    for (std::size_t i = 0; i < model.images.size(); i++) {
        auto& image = model.images[i];

        if (image.pixel_type != TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE || image.bits != 8 || image.as_is || image.component < 3 || image.component > 4) {
            Print::Warn("Invalid texture will not be loaded.");
            continue;
        }

        TextureFormat format;
        switch (image.component) {
        case 3:
            format = TextureFormat::eRGB;
            break;
        case 4:
            format = TextureFormat::eRGBA;
            break;
        default:
            Print::Warn("Invalid texture format.");
            continue;
        }
        Extent2D extent { static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height) };

        const std::span<const std::byte> bytes(reinterpret_cast<const std::byte*>(image.image.data()), image.image.size());

        auto texture = std::make_shared<Texture2D>(bytes, format, extent);
        AddSubResource(texture);
        _textures[i] = texture;
    }

    // Load materials
    // Here we are assuming a default material type.
    auto defaultMaterial = resourceSystem->Load<Material>("Resources/Materials/Default.json");
    auto defaultSampler = resourceSystem->Load<Sampler>("Resources/Samplers/Default.json");

    for (auto& material : model.materials) {
        auto instance = defaultMaterial.lock()->CreateInstance();
        instance->SetBool("material.useTriplanar", false);
        AddSubResource(instance);

        if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
            auto texture = _textures[material.pbrMetallicRoughness.baseColorTexture.index];
            instance->SetSampledTexture2D("inAlbedo", defaultSampler, texture);
        }

        _materials.push_back(instance);
    }

    if (model.materials.size() == 0) {
        auto instance = defaultMaterial.lock()->CreateInstance();
        instance->SetBool("material.useTriplanar", false);
        AddSubResource(instance);

        _materials.push_back(instance);
    }

    // Load meshes
    _meshes.reserve(model.meshes.size());
    for (std::size_t i = 0; i < model.meshes.size(); i++) {
        auto& mesh = model.meshes[i];

        auto& primitive = mesh.primitives[0];
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
            std::memcpy(indices.data(), indexBuffer.data.data() + indexView.byteOffset, 4 * indexAccessor.count);
        } else {
            for (std::size_t j = 0; j < indexAccessor.count; j++) {
                std::memcpy(&indices[j], indexBuffer.data.data() + indexView.byteOffset + (j * indicesWidth), indicesWidth);
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
                for (std::size_t j = 0; j < attribAccessor.count; j++) {
                    // sorta unsafe
                    size_t offset = bufferView.byteOffset + (j * sizeof(glm::vec3));
                    std::memcpy(&vertices[j].position, &buffer.data[offset], sizeof(glm::vec3));
                }
            }

            if (attrib.first == "NORMAL") {
                if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || attribAccessor.type != TINYGLTF_TYPE_VEC3) {
                    throw std::runtime_error("Invalid normal type.");
                }
                for (std::size_t j = 0; j < attribAccessor.count; j++) {
                    // sorta unsafe
                    size_t offset = bufferView.byteOffset + (j * sizeof(glm::vec3));
                    std::memcpy(&vertices[j].normal, &buffer.data[offset], sizeof(glm::vec3));
                }
            }

            if (attrib.first == "TEXCOORD_0") {
                if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || attribAccessor.type != TINYGLTF_TYPE_VEC2) {
                    throw std::runtime_error("Invalid normal type.");
                }
                for (std::size_t j = 0; j < attribAccessor.count; j++) {
                    // sorta unsafe
                    size_t offset = j * sizeof(glm::vec2) + bufferView.byteOffset;
                    std::memcpy(&vertices[j].texCoords, &buffer.data[offset], sizeof(glm::vec2));
                }
            }
        }

        auto m = std::make_shared<Mesh>();
        AddSubResource(m);
        _meshes.push_back(m);
        m->Upload<Vertex>(vertices, indices);
    }

    // model.textures[0].source

    // Build out the scene tree for model loading.
    const auto& scene = model.scenes[model.defaultScene];

    _root = std::make_unique<Node3D>();

    for (int i : scene.nodes) {
        _root->AddChild(LoadNode(model, model.nodes[i]));
    }
}

void Model::Release()
{
    Resource::Release();
}

void Model::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Model", "Resource", &Model::Create);
}

}