#pragma once

#include "Device.h"
#include "Graphics/Resource/Buffer.h"

namespace bl
{

class BLUEMETAL_API Mesh
{
public:
    template<typename TVertex>
    Mesh(Device* device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);

    template<typename TVertex>
    void setVertices(const std::vector<TVertex>& vertices);
    void setIndices(const std::vector<uint32_t>& indices);

    void bind(VkCommandBuffer cmd);

private:
    Device* _device;
    std::unique_ptr<Buffer> _vertexBuffer;
    std::unique_ptr<Buffer> _indexBuffer;
};

} // namespace bl

#include "Mesh.inl"
