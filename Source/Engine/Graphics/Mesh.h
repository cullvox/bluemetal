#pragma once

#include "Device.h"
#include "Buffer.h"
#include "Vertex.h"

namespace bl
{

class BLUEMETAL_API Mesh
{
public:
    template<typename TVertex>
    Mesh(std::shared_ptr<Device> device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);
    
    template<typename TVertex>
    void setVertices(const std::vector<TVertex>& vertices);
    void setIndices(const std::vector<uint32_t>& indices);

private:
    std::shared_ptr<Device> _device;
    std::unique_ptr<Buffer> _vertexBuffer;
    std::unique_ptr<Buffer> _indexBuffer;
};

#include "Mesh.inl"

} // namespace bl