#pragma once

#include <cstdint>

#include "Vertex.h"

namespace bl
{

enum class TextureType : uint32_t
{
    eAlbdeo,
};


struct ModelHeader
{
    static inline uint32_t ModelMagic = 0x424D4D46; // blue metal model format

    uint32_t magic;
    uint32_t numMeshes;
    // uint32_t numTextures;
};


struct TextureReference
{
    uint32_t textureIndex;
};

struct MeshHeader
{
    uint32_t numVertices;
    uint32_t numIndices;
    //uint32_t numTextureReferences;

    // Below In Bytes
        // vertices - sizeof(Vertex) * numVertices
        // indices - sizeof(uint32_t) * numIndices
        // modelMatrix - sizeof(float[16])
        // textureReferences - sizeof(uint32_t) * numTextureReferences
};

struct ModelMatrix
{
    float a1, a2, a3, a4;
    float b1, b2, b3, b4;
    float c1, c2, c3, c4;
    float d1, d2, d3, d4;
};

struct TextureHeader
{
    TextureType type;
    uint32_t numBytes;
};



}