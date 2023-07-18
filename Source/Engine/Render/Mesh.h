#pragma once

#include "Device.h"
#include "Buffer.h"
#include "Vertex.h"

class BLUEMETAL_API blMesh
{
public:
    template<typename TVertex>
    explicit blMesh(std::shared_ptr<blDevice> device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);
    
    template<typename TVertex>
    void setVertices(const std::vector<TVertex>& vertices);
    void setIndices(const std::vector<uint32_t>& indices);

private:
    std::shared_ptr<blDevice> _device;
    std::unique_ptr<blBuffer> _vertexBuffer;
    std::unique_ptr<blBuffer> _indexBuffer;
};

#include "Mesh.inl"
