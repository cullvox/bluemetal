#pragma once

#include <cstdint>

#include "Vertex.h"

namespace bl
{

struct ModelHeader
{
    uint32_t magic;
    uint32_t numVertexBuffers;
    uint32_t numIndexBuffers;
    uint32_t numTransforms;
    uint32_t numAnimations;
    uint32_t numSkeletons;
};

struct VertexHeader
{
    uint32_t numVertices;
};

struct IndexHeader
{
    uint32_t numIndices;
};

struct ModelMatrix
{
    float a1, a2, a3, a4;
    float b1, b2, b3, b4;
    float c1, c2, c3, c4;
    float d1, d2, d3, d4;
};

struct ModelNode
{
    uint32_t parentTransformIndex;
    ModelMatrix matrix;

}

}