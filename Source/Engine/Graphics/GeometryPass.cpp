///////////////////////////////
// Headers
///////////////////////////////

#include "GeometryPass.h"
#include "Core/Log.h"

namespace bl {

///////////////////////////////
// Classes
///////////////////////////////

GeometryPass::GeometryPass(GraphicsDevice* pDevice, VkExtent2D extent)
    : m_pDevice(pDevice)
    , m_extent(extent)
{
}

GeometryPass::~GeometryPass()
{
    destroyPass();
    destroyFramebuffer();
    destroyImages();
}

bool GeometryPass::create(const GeometryPassCreateInfo& createInfo)
{
    assert(createInfo.pDevice != nullptr && "GeometryPassCreateInfo.pDevice must not be nullptr");
    assert(createInfo.imageCount > 0
        && "GeometryPassCreateInfo.imageCount must be greater than zero!");
    assert(createInfo.extent.width > 0 && createInfo.extent.height > 0
        && "GeometryPassCreateInfo.extent width and height must be greater than zero!");

    auto physicalDevice = m_pDevice->getPhysicalDevice();
    m_albedoSpecularFormat = physicalDevice->findSupportedFormat(
        { VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_UNORM }, VK_IMAGE_TILING_OPTIMAL, 0);
    m_positionFormat = physicalDevice->findSupportedFormat(
        { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16_SFLOAT,
            VK_FORMAT_R16G16B16A16_SFLOAT },
        VK_IMAGE_TILING_OPTIMAL, 0);
    m_normalFormat = physicalDevice->findSupportedFormat(
        { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16_SFLOAT,
            VK_FORMAT_R16G16B16A16_SFLOAT },
        VK_IMAGE_TILING_OPTIMAL, 0);

    m_albedoSpecularImages.reserve(createInfo.imageCount);
    m_positionImages.reserve(createInfo.imageCount);
    m_normalImages.reserve(createInfo.imageCount);

    createImages();
    createFramebuffer();
    createPass();
}

bool GeometryPass::createImages()
{

    for (uint32_t i = 0; i < m_imageCount; i++) {
        m_albedoSpecularImages.push_back(std::make_unique<Image>(m_pDevice, VK_IMAGE_TYPE_2D,
            m_albedoSpecularFormat, VkExtent3D { m_extent.width, m_extent.height, 1 },
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1));

        m_positionImages.pushB
    }

    m_albedoSpecular = ;
}

void GeometryPass::destroyImages() { }

bool GeometryPass::createFramebuffer() { }

void GeometryPass::destroyFramebuffer() { }

bool GeometryPass::createPass()
{
    std::array<VkAttachmentDescription, 3> attachments = {};
    attachments[0].flags = 0;
    attachments[0].format = m_pSwapchain->getFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference presentAttachmentReference = {};
    presentAttachmentReference.attachment = 0;
    presentAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::array colorAttachments = { presentAttachmentReference };

    std::array<VkSubpassDescription, 1> subpasses = {};
    subpasses[0].flags = {};
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = (uint32_t)colorAttachments.size();
    subpasses[0].pColorAttachments = colorAttachments.data();
    subpasses[0].pResolveAttachments = nullptr;
    subpasses[0].pDepthStencilAttachment = nullptr;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = (uint32_t)attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = (uint32_t)subpasses.size();
    createInfo.pSubpasses = subpasses.data();
    createInfo.dependencyCount = 0;
    createInfo.pDependencies = nullptr;

    if (vkCreateRenderPass(m_pDevice->getHandle(), &createInfo, nullptr, &m_pass) != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Vulkan present render pass!");
    }
}

void GeometryPass::destroyPass() { }

} // namespace bl