#include "PresentRenderPass.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_bluemetal.h"

namespace bl
{

PresentRenderPass::PresentRenderPass(std::shared_ptr<Device> device, std::shared_ptr<Swapchain> swapchain)
    : m_device(device)
    , m_swapchain(swapchain)
{
    createRenderPass();
    createFramebuffers();
}

PresentRenderPass::~PresentRenderPass()
{
    destroyFramebuffers();
    destroyRenderPass();
}

VkRenderPass PresentRenderPass::getHandle()
{
    return m_pass;
}

void PresentRenderPass::resize(VkExtent2D extent)
{
    (void)extent;
    destroyFramebuffers();
    createFramebuffers();
}

//Helper to display a little (?) mark which shows a tooltip when hovered.
//In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void PresentRenderPass::record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t index)
{
    std::array<VkClearValue, 2> clearValues = {};

    VkClearColorValue value = {};
    value.float32[0] = 0.33f;
    value.float32[1] = 0.34f;
    value.float32[2] = 0.34f;
    value.float32[3] = 1.0f;

    clearValues[0].color = value;
    clearValues[1].depthStencil = VkClearDepthStencilValue(1.0f, 0);

    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = m_pass;
    beginInfo.framebuffer = m_swapFramebuffers[index];
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = (uint32_t)clearValues.size();
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    // if (!m_showImGui) return;

    ImGui_ImplBluemetal_BeginFrame();

    auto currentPresentMode = m_swapchain->getPresentMode();
    auto currentSurfaceFormat = m_swapchain->getSurfaceFormat();

    ImGui::Begin("Debug Info");
    // ImGui::Text("Graphics Device: %s", m_physicalDevice->getDeviceName().c_str());
    // ImGui::Text("Graphics Vendor: %s", m_physicalDevice->getVendorName().c_str());
    // ImGui::Text("F/S: %d", _frameCounter.getFramesPerSecond());
    // ImGui::Text("MS/F: %.2f", _frameCounter.getMillisecondsPerFrame());
    // ImGui::Text("Average F/S (Over 10 Seconds): %.1f", _frameCounter.getAverageFramesPerSecond(10));
    // ImGui::Text("Average MS/F (Over 144 Frames): %.2f", _frameCounter.getAverageMillisecondsPerFrame(144));
    // ImGui::Text("Present Mode: %s", string_VkPresentModeKHR(currentPresentMode));
    // ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));

    ImGui::End();

    ImGui::Begin("Advanced Configuration");

    ImGui::SeparatorText("ImGui Interface");

    static float alpha = 1.0f;
    if (ImGui::SliderFloat("UI Alpha Transparency", &alpha, 0.1f, 1.0f))
    {
        ImGui::GetStyle().Alpha = alpha;
    }

    ImGui::SeparatorText("Graphics");


    // if (ImGui::CollapsingHeader("Physical Device"))
    // {
    //     for (size_t i = 0; i < physicalDevices)
    // }

    // if (ImGui::CollapsingHeader("Surface Formats"))
    // {
// 
    //     static int surfaceFormatSelected = 0;
    //     for (size_t i = 0; i < surfaceFormats.size(); i++)
    //     {
    //         HelpMarker("Changes to this value will require a restart to change.");
    //         std::string name = fmt::format("({}, {})", string_VkFormat(surfaceFormats[i].format), string_VkColorSpaceKHR(surfaceFormats[i].colorSpace));
    //         if (ImGui::RadioButton(name.c_str(), &surfaceFormatSelected, i))
    //         {
    //             m_config["render"]["presentMode"] = (int)i;
    //         }
    //     }
    // }
// 
    // if (ImGui::CollapsingHeader("Present Modes"))
    // {
    //     static int selectedPresentMode = 0;
    //     for (size_t i = 0; i < presentModes.size(); i++)
    //     {
    //         std::string name = fmt::format("({})", string_VkPresentModeKHR(presentModes[i]));
    //         if (ImGui::RadioButton(name.c_str(), &selectedPresentMode, i))
    //         {
    //             static int value = 0;
    //             value = i;
    //             _postRenderCommands.push([&](){
    //                 m_swapchain->recreate(presentModes[value], m_swapchain->getSurfaceFormat());
    //                 m_renderer->resize(m_swapchain->getExtent());
    //             });
    //         }
    //     }
    // }

    ImGui::End();

    ImGui::ShowDemoWindow();
    
    ImGui_ImplBluemetal_EndFrame(cmd);

    vkCmdEndRenderPass(cmd);
}

void PresentRenderPass::createRenderPass()
{
    std::array<VkAttachmentDescription, 1> attachments = {};
    attachments[0].flags = 0;
    attachments[0].format = m_swapchain->getFormat();
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

    if (vkCreateRenderPass(m_device->getHandle(), &createInfo, nullptr, &m_pass) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan present render pass!");
    }

}

void PresentRenderPass::destroyRenderPass()
{
    vkDestroyRenderPass(m_device->getHandle(), m_pass, nullptr);
}

void PresentRenderPass::createFramebuffers()
{
    // Get the image views from the swapchain.
    std::vector<VkImageView> attachments = m_swapchain->getImageViews();
    
    // Get the extent of the swapchain for framebuffer sizes.
    VkExtent2D extent = m_swapchain->getExtent();

    m_swapFramebuffers.resize(attachments.size());

    // Create some basic info for the loop and fill in the value needed in the loop.
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.renderPass = m_pass;
    createInfo.attachmentCount = 1;
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = 1;

    // Iterate through all the attachments to create their framebuffer.
    uint32_t i = 0;
    for (VkImageView attachment : attachments)
    {
        
        // Set the attachment and create the framebuffer.
        createInfo.pAttachments = &attachment;
        
        if (vkCreateFramebuffer(m_device->getHandle(), &createInfo, nullptr, &m_swapFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create a Vulkan framebuffer!");
        }

        // Iterate the index for the next framebuffer in m_swapFramebuffers.
        i++;
    }
}

void PresentRenderPass::destroyFramebuffers()
{
    for (size_t i = 0; i < m_swapFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(m_device->getHandle(), m_swapFramebuffers[i], nullptr);
    }

    m_swapFramebuffers.clear();
}

} // namespace bl