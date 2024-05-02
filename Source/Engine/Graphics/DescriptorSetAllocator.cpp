#include "DescriptorSetAllocator.h"

namespace bl 
{

DescriptorSetAllocator::DescriptorSetAllocator(std::shared_ptr<GfxDevice> device)
    : _device(device)
{
    
}

DescriptorSetAllocator::~DescriptorSetAllocator()
{

}

} // namespace bl