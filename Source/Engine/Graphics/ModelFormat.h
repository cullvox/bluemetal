#pragma once

#include <cstdint>

#include "Vertex.h"

namespace bl
{

static inline uint32_t ModelMagic = 0x424D4D46; // blue metal model format

struct ModelHeader
{
    uint32_t magic;
    uint32_t numMeshes;
};

struct MeshHeader
{
    uint32_t numVertices;
    uint32_t numIndices;
};

struct TransformHeader
{
    uint32_t numTransforms;
};

struct ModelMatrix
{
    float a1, a2, a3, a4;
    float b1, b2, b3, b4;
    float c1, c2, c3, c4;
    float d1, d2, d3, d4;
};

}