#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Image.hpp"
#include "Render/Swapchain.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Pipeline.hpp"

struct Submission
{
    blPipeline* pipeline;
    blBuffer* indexBuffer;
    blBuffer* vertexBuffer;
    int instanceCount;
};

class BLUEMETAL_RENDER_API blRenderer
{
    static const inline uint32_t MAX_FRAMES_IN_FLIGHT = 2;
public:
    blRenderer(std::shared_ptr<const blRenderDevice> renderDevice,
        std::shared_ptr<blSwapchain> swapchain,
        const std::vector<std::shared_ptr<blRenderPass>>& passes);
    ~blRenderer();

    void resize(vk::Extent2D extent);
    void submit(const Submission& submission) noexcept;
    void render();

private:
    void createSyncObjects();
    void destroySyncObjects() noexcept;

    uint32_t _currentFrame;
    std::vector<vk::CommandBuffer> _swapCommandBuffers;
    std::vector<vk::Semaphore> _imageAvailableSemaphores;
    std::vector<vk::Semaphore> _renderFinishedSemaphores;
    std::vector<vk::Fence> _inFlightFences;
    std::shared_ptr<const blRenderDevice> _renderDevice;
    std::shared_ptr<blSwapchain> _swapchain;
    const std::vector<std::shared_ptr<blRenderPass>> _passes;  
};