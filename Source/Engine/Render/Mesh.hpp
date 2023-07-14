#pragma once

#include "Render/Buffer.hpp"
#include "Render/Vertex.hpp"
#include "Render/Export.h"

class BLUEMETAL_API blMesh
{
public:
template<typename Integer,
             

    template<typename TVertex>
    explicit blMesh(std::shared_ptr<blRenderDevice> renderDevice, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);
    
    template<typename TVertex>
    void setVertices(const std::vector<TVertex>& vertices);

    void setIndices(const std::vector<uint32_t>& indices);

private:
    std::shared_ptr<blRenderDevice> _renderDevice;
    std::unique_ptr<blBuffer> _vertexBuffer;
    std::unique_ptr<blBuffer> _indexBuffer;
};

#include "Mesh.inl"
