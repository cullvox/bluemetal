#include "VulkanMaterial.h"
#include "Core/Print.h"
#include "VulkanBuffer.h"
#include "UniformData.h"
#include "Renderer.h"
#include <memory>

namespace bl {

VulkanMaterial::VulkanMaterial(VulkanDevice* device, Renderer* renderer, const VulkanPipelineStateInfo& state, int32_t materialSet)
    : VulkanMaterialInstance(device)
    , _descriptorSetCache(device, 1024, VulkanDescriptorRatio::Default())
    , _flags(VulkanMaterialSupportFlags::eNone)
{
    _material = this;
    _materialSet = materialSet;

    // Preform reflection on the pipeline shaders to retrieve detailed descriptor set info.
    auto reflection = VulkanReflectedPipeline::Reflect(state.stages);

    // // If the material contains the global descriptor set, we need to modify it to use dynamic uniform buffers.
    // if (reflection.descriptorSetMetadata.contains(0)) {
    //     auto& globalSet = reflection.descriptorSetMetadata.at(0);
    //     auto& globalBindings = globalSet.GetBindings();

    //     for (auto& pair : globalBindings) {
    //         auto binding = pair.second.GetBinding();

    //         if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
    //             pair.second.SetType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
    //         }
    //     }
    // }

    auto& sets = reflection.descriptorSetMetadata;
    if (!sets.contains(materialSet) && materialSet != -1)
        throw std::runtime_error("VulkanMaterial does not contain the used set!");

    if (materialSet != -1) {
        _flags |= VulkanMaterialSupportFlags::eMaterialBuffer;
    }

    // Determine if this pipeline layout uses the global uniform buffer.
    if (sets.contains(0) && sets[0].Contains(0)) {
        auto& globalSet = sets[0];
        auto& binding = globalSet[0];
        if (binding.IsBlock() &&
            binding.GetName() == "viewport" &&
            binding.GetSize() == sizeof(ViewportUBO)) {
            _flags |= VulkanMaterialSupportFlags::eGlobalBuffer;
        }
    }

    if (sets.contains(1) && sets[1].Contains(0)) {
        auto& instanceSet = sets[1];
        auto& binding = instanceSet[0];
        if (binding.GetType() == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
            _flags |= VulkanMaterialSupportFlags::eInstanceBuffer;
        }
    }


    if (sets.contains(2) && sets[2].Contains(0)) {
        auto& materialSet = sets[2];
        auto& binding = materialSet[0];
        if (binding.GetType() != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            Print::Error("Descriptor (set=2, binding=0) must be a uniform buffer!");
        }
    }

    _setCount = reflection.GetPipelineRequiredSetCount();

    // Construct the pipeline.
    _pipeline = std::make_unique<VulkanPipeline>(device, renderer, state, &reflection);

    if (materialSet == -1) return;

    const auto& pipelineDescriptorSetLayouts = _pipeline->GetDescriptorSetLayouts();
    _layout = pipelineDescriptorSetLayouts.at(materialSet);

    BuildPerFrameBindings(_layout);

    _emptySet = _descriptorSetCache.Allocate(_device->AcquireDescriptorSetLayout({}));

    // Create buffers/sampler uniform data.

    auto& meta = sets.at(materialSet);
    auto metaBindings = meta.GetMetaBindings();
    for (const auto& binding : metaBindings) {
        switch (binding.GetType()) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            auto blocks = binding.GetMembers();

            // Get each uniform members block for its offsets.
            for (const auto& variable : blocks) {
                _uniforms[variable.GetName()] = variable;
            }
            break;
        }
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            _samplers[binding.GetName()] = binding.GetLocation();
            break;
        default:
            break;
        }
    }

    _instances.push_back(this);
}

VulkanMaterial::~VulkanMaterial()
{
    // Destroy all descriptor set data, the remaining instances will not be bindable.
    // Binding them will cause an exception even if their pointers remain available.

    for (auto instance : _instances) {
        instance->FreeSets();
    }
}

VulkanMaterialSupportFlags VulkanMaterial::GetSupportFlags() const
{
    return _flags;
}

std::unique_ptr<VulkanMaterialInstance> VulkanMaterial::CreateInstance()
{
    auto instance = std::make_unique<VulkanMaterialInstance>(_device, this);
    _instances.push_back(instance.get());
    return instance;
}

} // namespace bl
