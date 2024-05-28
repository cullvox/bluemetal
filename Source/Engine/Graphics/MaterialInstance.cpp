#include "MaterialInstance.h"

namespace bl
{

void MaterialInstance::CreateBuffers()
{

    auto sets = _material->GetPipeline()->GetDescriptorSetReflections();
    
    for (const auto& set : sets)
    {
        auto bindings = set.GetBindingReflections();

        for (const auto& binding : bindings)
        {
            VkWriteDescriptorSet write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.pNext = nullptr;
            write.dstSet = _set;
            write.dstBinding = 0;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = {};
            write.pImageInfo = nullptr;
            write.pBufferInfo = nullptr;
            write.pTexelBufferView = nullptr;

            switch (binding.GetType())
            {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

                // Allocate a new uniform buffer.
                Buffer buffer{_device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.GetBlockSize() };

                VkDescriptorBufferInfo info = {};
                info.buffer = buffer.Get();
                info.offset = 0;
                info.range = VK_WHOLE_SIZE;

                _uniformBuffers.emplace(binding.GetBlockName(), buffer);
                auto members = binding.GetMembers();
                break;
            default:
                continue; // These descriptor types aren't yet managed by the material system.
            }

            vkUpdateDescriptorSets(_device->Get(), 1, &write, 0, nullptr);
        }
    }

}

void MaterialInstance::UpdateDescriptor()
{


}

} // namespace bl