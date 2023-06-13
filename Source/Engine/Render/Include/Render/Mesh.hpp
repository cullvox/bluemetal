#pragma once

#include "Render/Buffer.hpp"
#include "Render/Vertex.hpp"
#include "Render/Export.h"

class BLUEMETAL_RENDER_API blMesh
{
public:
    blMesh(std::shared_ptr<blRenderDevice> renderDevice, const std::vector<blVertex>& vertices, const std::vector<uint32_t>& indices); // creates a mesh using a vertex/indices array
    blMesh(blMesh&) = delete;
    ~blMesh() = default;

private:
    std::shared_ptr<blRenderDevice> _renderDevice;
    blBuffer _vertexBuffer;
    blBuffer _indexBuffer;
};