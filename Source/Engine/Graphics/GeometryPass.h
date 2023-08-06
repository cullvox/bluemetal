#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Device.h"
#include "Image.h"
#include "RenderPass.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

/// A render pass containing images for geometry d
class BLUEMETAL_API GeometryPass : public RenderPass
{
public:
    GeometryPass(GraphicsDevice* pDevice, VkExtent2D extent);
    ~GeometryPass();

    virtual VkRenderPass getHandle();
    virtual void resize(VkExtent2D extent); // resizes images, framebuffers to match extent 
    virtual void record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex);

private:
    void createImages();
    void destroyImages();
    void createFramebuffer();
    void destroyFramebuffer();
    void createPass();
    void destroyPass();

    GraphicsDevice*         m_pDevice;
    VkExtent2D              m_extent;
    std::unique_ptr<Image>  m_albedoSpecular;
    std::unique_ptr<Image>  m_position;
    std::unique_ptr<Image>  m_normal;
    VkFramebuffer           m_gBuffer;
};

}