#include "Core/Print.h"
#include "GeometryPass.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>

namespace bl {

GeometryPass::GeometryPass(Device* device)
    : _device(device) {

    
}

GeometryPass::~GeometryPass() {
    DestroyImagesAndFramebuffers();
    vkDestroyRenderPass(_device->Get(), _pass, nullptr);
}

void GeometryPass::Recreate(VkExtent2D extent, uint32_t imageCount) {
    
}

void GeometryPass::Begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) {

    std::array clearColors = {
        VkClearValue{ .color = {0.0f, 0.0f, 0.0f, 0.0f}}, // Light Clear Color
        VkClearValue{ .depthStencil = {0.0f, 0}}, // Depth Clear Color
        VkClearValue{ .color = {0.1f, 0.4f, 0.4f, 1.0f}}, // Albedo Clear Color
        VkClearValue{ .color = {0.0f, 0.0f, 0.0f, 0.0f}} // Normal Clear Color
    };

    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = _pass;
    beginInfo.framebuffer = _framebuffers[imageIndex];
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = (uint32_t)clearColors.size();
    beginInfo.pClearValues = clearColors.data();

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void GeometryPass::End(VkCommandBuffer cmd) {
    vkCmdEndRenderPass(cmd);
}

void GeometryPass::DestroyImagesAndFramebuffers() {
    for (VkFramebuffer fb : _framebuffers)
        vkDestroyFramebuffer(_device->Get(), fb, nullptr);

    _framebuffers.clear();
    _lightImages.clear();
    _depthImages.clear();
    _albedoImages.clear();
    _normalImages.clear();
}

} // namespace bl