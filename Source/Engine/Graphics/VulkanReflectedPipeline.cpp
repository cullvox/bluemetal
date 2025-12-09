#include <fmt/format.h>

#include "Core/Print.h"
#include "VulkanShader.h"
#include "VulkanReflectedPipeline.h"

namespace bl {

// Recursively reflects members of a block variable into the given reflected block.
// Helper function for VulkanReflectedPipeline::Reflect.
static void ReflectMembers(VulkanReflectedBlock& meta, uint32_t binding, const SpvReflectBlockVariable& block, std::string parent = "")
{
    std::string structName;
    if (parent.empty()) {
        structName = block.name;
    } else {
        structName = fmt::format("{}.{}", parent, block.name);
    }

    meta.SetName(structName);

    for (uint32_t j = 0; j < block.member_count; j++) {
        auto& blockVariable = block.members[j];
        auto& numericTraits = blockVariable.numeric;
        auto typeDescription = blockVariable.type_description;
        VulkanVariableBlockType type = VulkanVariableBlockType::eScalarInt;

        if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT) {
            ReflectMembers(meta, binding, blockVariable, meta.GetName());
            continue;
        }

        // We are a little specific about our supported material uniform block types.
        if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_ARRAY) {
            Print::Warn("Arrays are not supported in pipelines, it will not be parameterized.");
            continue;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {

            // We only support floating vector types.
            if (!(typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)) {
                Print::Warn("Only float vectors are supported in pipelines, {} in {} will not be parameterized.", typeDescription->struct_member_name, blockVariable.name);
                continue;
            }
            std::array types = { VulkanVariableBlockType::eVector2, VulkanVariableBlockType::eVector3, VulkanVariableBlockType::eVector4 };
            type = types[numericTraits.vector.component_count - 2];
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {

            // We only support 4x4 matrices.
            if (numericTraits.matrix.column_count != 4 || numericTraits.matrix.row_count != 4) {
                Print::Warn("Only 4x4 matrices are supported in pipelines, {} in {} will not be parameterized.", typeDescription->struct_member_name, blockVariable.name);
                continue;
            }
            type = VulkanVariableBlockType::eMatrix4;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL) {
            type = VulkanVariableBlockType::eScalarBool;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
            type = VulkanVariableBlockType::eScalarInt;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
            type = VulkanVariableBlockType::eScalarFloat;
        }

        std::string name = fmt::format("{}.{}", structName, blockVariable.name);

        meta[name]
            .SetName(name)
            .SetBinding(binding)
            .SetOffset(blockVariable.offset)
            .SetType(type)
            .SetSize(blockVariable.size);
    }
}

VulkanReflectedPipeline VulkanReflectedPipeline::Reflect(const VulkanPipelineStateInfo::Stages& state)
{
    const auto& shaders = state.shaders;
    VulkanReflectedPipeline reflection;

    // Obtain reflection data from shaders to build the pipeline layout.
    for (size_t i = 0; i < shaders.size(); i++) {

        // Reflect each descriptor binding to build descriptor set layouts.
        VulkanShader* shader = shaders[i];
        auto spirvReflection = shader->GetReflection();

        for (uint32_t j = 0; j < spirvReflection.descriptor_binding_count; j++) {
            const auto& reflectBinding = spirvReflection.descriptor_bindings[j];
            auto& set = reflection.descriptorSetMetadata[reflectBinding.set];

            set.SetLocation(reflectBinding.set);

            // If this binding number already exists then compare and use that.
            auto location = reflectBinding.binding;
            auto type = static_cast<VkDescriptorType>(reflectBinding.descriptor_type);
            auto count = reflectBinding.count;

            // If the set already contains this binding we do less work!
            bool doesBindingAlreadyExists = set.Contains(location);
            auto& binding = set[location];

            if (doesBindingAlreadyExists && !binding.Compare(type, count)) {
                throw std::runtime_error("Bindings using same index but hold different types of data!");
            } else {
                // This is a newly found binding
                binding.SetBinding(location, type, count, shader->GetStage(), nullptr);
                if (binding.IsBlock()) {
                    ReflectMembers(binding, location, reflectBinding.block);
                } else {
                    binding.SetName(reflectBinding.name);
                }
            }

            binding
                .AddStageFlags(shader->GetStage())
                .SetSize(reflectBinding.block.size);
        }

        // Gather all the push constant ranges for the pipeline layout.
        // Check if the push constant already exists.
        for (uint32_t j = 0; j < spirvReflection.push_constant_block_count; j++) {
            const auto& block = spirvReflection.push_constant_blocks[j];
            auto offset = block.offset;
            auto size = block.size;

            auto it = std::find_if(reflection.pushConstantMetadata.begin(), reflection.pushConstantMetadata.end(),
                [offset, size](const auto& refl) { return refl.Compare(offset, size); });

            if (it != reflection.pushConstantMetadata.end()) {
                // Add the stage to the existing push constant range.
                auto& refl = (*it);
                refl.AddStageFlags(shader->GetStage());
            } else {
                // This block wasn't added yet.
                auto& pcm = reflection.pushConstantMetadata.emplace_back(shader->GetStage(), offset, size);
                ReflectMembers(pcm, 0, block);
            }
        }
    }

    return reflection;
}

}