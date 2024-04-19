#pragma once

#include "Device.h"
#include "Buffer.h"

namespace bl
{

class BLUEMETAL_API Mesh
{
public:
    template<typename TVertex>
    Mesh(std::shared_ptr<GfxDevice> device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);

    template<typename TVertex>
    void setVertices(const std::vector<TVertex>& vertices);
    void setIndices(const std::vector<uint32_t>& indices);

    void bind(VkCommandBuffer cmd);

private:
    std::shared_ptr<GfxDevice> _device;
    std::unique_ptr<GfxBuffer> _vertexBuffer;
    std::unique_ptr<GfxBuffer> _indexBuffer;
};

} // namespace bl

#include "Mesh.inl"
