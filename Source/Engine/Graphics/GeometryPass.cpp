///////////////////////////////
// Headers
///////////////////////////////

#include "Core/Log.h"
#include "GeometryPass.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

GeometryPass::GeometryPass(GraphicsDevice* pDevice, VkExtent2D extent)
    : m_pDevice(pDevice)
    , m_extent(extent)
{
    createImages();
    createFramebuffer();
    createPass();
}

GeometryPass::~GeometryPass()
{
    destroyPass();
    destroyFramebuffer();
    destroyImages();
}

void GeometryPass::createImages()
{
    auto physicalDevice = m_pDevice->getPhysicalDevice();
    auto albedoSpecFormat = physicalDevice->findSupportedFormat({VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_UNORM}, VK_IMAGE_TILING_OPTIMAL, 0);
    // auto positionFormat = physicalDevice->findSupportedFormat({VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R16G16B16_SFLOAT}, VK_IMAGE_TILING_OPTIMAL, 0);
    // auto normalFormat = physicalDevice->findSupportedFormat({VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R16G16B16_SFLOAT}, VK_IMAGE_TILING_OPTIMAL, 0);

    BL_LOG(LogType::eInfo, "Creating Albedo-Specular geometry pass image with format: {}", string_VkFormat(albedoSpecFormat));

    m_albedoSpecular = std::make_unique<Image>(
        m_pDevice, 
        VK_IMAGE_TYPE_2D, 
        albedoSpecFormat, 
        VkExtent3D{m_extent.width, m_extent.height, 1}, 
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT, 
        1);
    


}

void GeometryPass::destroyImages()
{

}

void GeometryPass::createFramebuffer()
{

}

void GeometryPass::destroyFramebuffer()
{

}

void GeometryPass::createPass()
{

}

void GeometryPass::destroyPass()
{

}


} // namespace bl