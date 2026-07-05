#pragma once

#include "Core/Color.h"
#include "VulkanBufferFrameRing.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "Vertex.h"
#include <vulkan/vulkan_core.h>

namespace bl
{

class Node;
class Renderer;
class VulkanDevice;
class VulkanMaterialInstance;
class VulkanMesh;
class Viewport;

class RenderData
{
    Viewport* _viewport;
    VkCommandBuffer _cmd;
    uint32_t _currentFrame;
    uint32_t _imageIndex;
    VkDescriptorSet _globalSet;
    VkSampleCountFlagBits _sampleCount;
    VkImageView _swapchainImageView;
    uint32_t _nodeID;

    float _frameTime;
    float _deltaFrameTime;

    std::vector<glm::mat4> _tempInstances;
    std::vector<uint32_t> _instanceToCallMap;
    std::vector<glm::mat4> _instances;
    VulkanBufferFrameRing _instanceBuffer;
    VulkanDescriptorSetAllocatorCache _descriptorCache;
    VkDescriptorSetLayout _instanceSetLayout;
    std::array<std::unique_ptr<VulkanDescriptorSet>, VulkanConfig::maxFramesInFlight> _instanceSets;

    std::vector<VkSemaphoreSubmitInfo> _waitSemaphores; // The semaphores we're waiting on before rendering begins.
    std::vector<VkSemaphoreSubmitInfo> _signalSemaphores; // The semaphores we signal when rendering finishes.

    glm::mat4 _projection;
    glm::mat4 _view;

    struct DrawCall {
        DrawCall(const VulkanMaterialInstance* material, const VulkanMesh* mesh)
            : material(material)
            , mesh(mesh)
        {
        }

        const VulkanMaterialInstance* material;
        const VulkanMesh* mesh;
        uint32_t hash;
        uint32_t nodeID;
        uint32_t offset = 0;
        uint32_t count = 0;
    };

    std::vector<DrawCall> _calls;

    // Debug Rendering
    VulkanMaterialInstance*     _pointMaterial = nullptr;
    VulkanMaterialInstance*     _lineMaterial = nullptr;
    VulkanMaterialInstance*     _triangleMaterial = nullptr;
    std::vector<VertexDebug>    _points;
    std::vector<VertexDebug>    _lines;
    std::vector<VertexDebug>    _triangles;
    std::vector<VertexDebug>    _debugVertices;
    VulkanBufferFrameRing       _debugBuffer;

    void CreateDebugBuffer(VulkanDevice* device);
    void UpdateDebugBuffers();
    void DrawDebugBuffers();

public:
    RenderData(Renderer* renderer);

    void SetCommandBuffer(VkCommandBuffer cmd);
    void SetCurrentFrame(uint32_t currentFrame);
    void SetImageIndex(uint32_t index);
    void SetGlobalDescriptorSet(VkDescriptorSet set);
    void SetSampleCount(VkSampleCountFlagBits sampleCount);
    void SetSwapchainImageView(VkImageView swapchainImageView);
    void SetDebugMaterialInstance(VulkanMaterialInstance* pointMaterial, VulkanMaterialInstance* lineMaterial, VulkanMaterialInstance* triangleMaterial);
    void SetProjectionMatrix(const glm::mat4& projection);
    void SetViewMatrix(const glm::mat4& viewMatrix);
    void SetCurrentFrameTime(float frameTime);
    void SetDeltaFrameTime(float deltaFrameTime);
    void AddRenderWaitSemaphore(const VkSemaphoreSubmitInfo& info);
    void AddRenderSignalSemaphore(const VkSemaphoreSubmitInfo& info);

    VkCommandBuffer GetCommandBuffer();
    uint32_t GetCurrentFrame();
    uint32_t GetImageIndex();
    VkSampleCountFlagBits GetSampleCount();
    VkImageView GetSwapchainImageView();
    VkDescriptorSet GetGlobalDescriptorSet();
    VkDescriptorSet GetInstanceDescriptorSet();
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetViewMatrix();
    Viewport* GetViewport();
    float GetCurrentFrameTime();
    float GetDeltaFrameTime();
    const std::vector<VkSemaphoreSubmitInfo>& GetRenderWaitSemaphores();
    const std::vector<VkSemaphoreSubmitInfo>& GetRenderSignalSemaphores();

    void Draw(const VulkanMaterialInstance* material, uint32_t vertexCount);
    void DrawInstance(Node* node, const VulkanMaterialInstance* material, const VulkanMesh* mesh, const glm::mat4& instance);
    void DrawMultiInstance(Node* node, const VulkanMaterialInstance* material, const VulkanMesh* mesh, const std::span<glm::mat4> instances);

    void DrawPoint(const glm::vec3& point, float size = 1.0f, Color color = Color::Violet());
    void DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness = 1.0f, Color color = Color::Violet());
    void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness = 1.0f, Color color = Color::Violet());

    void WriteInstanceBuffer();
    void WriteDrawCommands();
    void Reset();


};

}