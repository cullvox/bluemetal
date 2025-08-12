#pragma once

#include <cstdint>
#include "Vertex.h"

namespace bl {

/**
 * @enum BMMFTextureType
 * @brief Types of textures used in the Bluemetal Model Format (BMMF).
 *
 * This enum defines the types of textures that can be used in a model.
 */
enum class BMMFTextureType : uint32_t {
    eDiffuse, /** @brief RGBA8 Diffuse texture. */
    eSpecular, /** @brief RGBA8 Specular texture. */
    eNormal, /** @brief RGBA8 Normal texture. */
    eEmissive, /** @brief RGBA8 Emissive texture. */
    eRoughness, /** @brief R8 Roughness texture. */
    eMetallic, /** @brief R8 Metallic texture. */
    eAO, /** @brief R8 AO texture. */
    eCount // Total number of texture types.
};

static constexpr uint32_t BMMF_MAGIC = 0x424D4D46;  // blue metal model format (BMMF)

/**
 * @struct BMMFHeader
 * @brief Header for the Bluemetal Model Format (BMMF).
 *
 * This header contains metadata about the model, including magic number,
 * number of meshes, textures, and materials.
 */
struct BMMFHeader {
    uint32_t magic;
    uint32_t numNodes; // Number of nodes in the model hierarchy.
    uint32_t numMeshes;
    uint32_t numTextures;
    uint32_t numMaterials;
    uint32_t numStrings;
};

/*

    * Header
    * Nodes Array
    * Node Parent Indices Array (parent index for each node)
    * Meshes Array
    *   Vertex Buffer Array
    *   Index Buffer Array
    * Textures
    *   Texture Buffer Array
    * Materials Array

*/

// Following the header comes an array of indices for where at every index is the node and the value is the index of it's parent node.
// After the array comes the nodes.


/**
 * @struct BMMFMatrix
 * @brief Represents a 4x4 matrix in the Bluemetal Model Format.
 *
 * This structure is used to store transformation matrices for meshes.
 */
struct BMMFMatrix {
    float a1, a2, a3, a4;
    float b1, b2, b3, b4;
    float c1, c2, c3, c4;
    float d1, d2, d3, d4;
};

struct BMMFNode {
    BMMFMatrix transformation; // Transformation matrix for the node.
    uint32_t nodeIndex; // Index of this node.
    uint32_t meshesCount; // Number of meshes this node uses.
    uint32_t nameIndex; // Index of the node name in the string table.
};

/**
 * @struct BMMFMeshHeader
 * @brief Header for each mesh in the Bluemetal Model Format.
 *
 * This header contains metadata about the mesh, including number of vertices,
 * indices, and a model matrix for transformations.
 */
struct BMMFMeshHeader {
    uint32_t numVertices;
    uint32_t numIndices;
    uint32_t materialIndex; // Index into the ModelHeader::numMaterials array.
};

struct BMMFTextureHeader {
    BMMFTextureType type;
    uint32_t numBytes;
};

enum class BMMFMaterialParameterType : uint32_t {
    eInt,
    eFloat,
    eDouble,
    eString
};

struct BMMFMaterialParameter {
    uint32_t nameIndex; // Index into the string table for the parameter name.
    BMMFMaterialParameterType type; // Type of the parameter.
    union {
        int intValue;
        float floatValue;
        double doubleValue;
        uint32_t stringIndex; // Index into the string table for the string value.
    } value; // Value of the parameter.
};

/**
 * @struct ModelMaterial
 * @brief Represents a material used in a model.
 *
 * This structure holds indices to various texture types used in the material.
 * Set to -1 if the texture is not used.
 */
struct BMMFMaterial {
    uint32_t diffuseTextureIndex;
    uint32_t specularTextureIndex;
    uint32_t normalTextureIndex;
    uint32_t emissiveTextureIndex;
    uint32_t roughnessTextureIndex;
    uint32_t metallicTextureIndex;
    uint32_t aoTextureIndex;
};

}