#pragma once

#include "Precompiled.h"

namespace bl
{
    
enum class PipelineResourceType
{
    eInput,
    eOutput,
    eSampler,
    eCombinedImageSampler,
    eSampledImage,
    eStorageImage,
    eUniformTexelBuffer,
    eStorageTexelBuffer,
    eUniformBuffer,
    eStorageBuffer,
    eInputAttachment,
    ePushConstantBuffer,
};

struct PipelineMemberInfo
{
    uint32_t offset;
    uint32_t size;
    uint32_t vecSize;
    uint32_t columns;
    uint32_t arraySize;
    std::string name;
};

struct GfxPipelineResource
{
    VkShaderStageFlags stages;
    VkDescriptorType type;
    VkAccessFlags access;
    uint32_t set;
    uint32_t binding;
    uint32_t location;
    uint32_t inputAttachmentIndex;
    uint32_t vecSize;
    uint32_t columns;
    uint32_t arraySize;
    uint32_t offset;
    uint32_t size;
    std::string name;
    std::vector<PipelineMemberInfo> members;
};

}