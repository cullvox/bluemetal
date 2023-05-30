#pragma once

#include "Render/Buffer.hpp"
#include "Render/Vertex.hpp"
#include "Render/Export.h"

enum class blMeshGenerator
{
    eCubeU1,
    eCubeU5,
    eSphereU1,
    eSphereU5,
    ePlaneU5,
};

class BLUEMETAL_RENDER_API blMesh
{
public:
    blMesh(std::shared_ptr<blRenderDevice> renderDevice, blMeshGenerator generator); // Creates a mesh using a specific generator
    blMesh(std::shared_ptr<blRenderDevice> renderDevice, const std::vector<blVertex>& vertices, const std::vector<uint32_t>& indices); // Creates a mesh using a vertex/indices array
    ~blMesh() = default; // Destroy the mesh and all its contents

private:
    std::shared_ptr<blRenderDevice> _renderDevice;
    std::shared_ptr<blBuffer> _vertexBuffer;
    std::shared_ptr<blBuffer> _indexBuffer;
};