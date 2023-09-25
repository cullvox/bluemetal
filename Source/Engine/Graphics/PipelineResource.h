#pragma once

#include "Precompiled.h"

namespace bl
{
    
enum PipelineResourceType
{
    PIPELINE_RESOURCE_TYPE_INPUT,
    PIPELINE_RESOURCE_TYPE_OUTPUT,
    PIPELINE_RESOURCE_TYPE_SAMPLER,
    PIPELINE_RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER,
    PIPELINE_RESOURCE_TYPE_SAMPLED_IMAGE,
    PIPELINE_RESOURCE_TYPE_STORAGE_IMAGE,
    PIPELINE_RESOURCE_TYPE_UNIFORM_TEXEL_BUFFER,
    PIPELINE_RESOURCE_TYPE_STORAGE_TEXEL_BUFFER,
    PIPELINE_RESOURCE_TYPE_UNIFORM_BUFFER,
    PIPELINE_RESOURCE_TYPE_STORAGE_BUFFER,
    PIPELINE_RESOURCE_TYPE_INPUT_ATTACHMENT,
    PIPELINE_RESOURCE_TYPE_PUSH_CONSTANT_BUFFER,
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

struct PipelineResource
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