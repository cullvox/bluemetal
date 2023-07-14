#pragma once

#include "Render/Precompiled.hpp"



struct blVulkanFormat
{

    // This format size is incomplete and will return 0 if it
    static inline uint32_t getSize(vk::Format format)
    {
        switch (format)
        {
            case vk::Format::eUndefined: return 0;
            case vk::Format::eR4G4UnormPack8: return 1;
            case vk::Format::eR4G4B4A4UnormPack16: return 2;
            case vk::Format::eB4G4R4A4UnormPack16: return 2;
            case vk::Format::eR5G6B5UnormPack16: return 2;
            case vk::Format::eB5G6R5UnormPack16: return 2;
            case vk::Format::eR5G5B5A1UnormPack16: return 2;
            case vk::Format::eB5G5R5A1UnormPack16: return 2;
            case vk::Format::eA1R5G5B5UnormPack16: return 2;
            case vk::Format::eR8Unorm: return 1;
            case vk::Format::eR8Snorm: return 1;
            case vk::Format::eR8Uscaled: return 1;
            case vk::Format::eR8Sscaled: return 1;
            case vk::Format::eR8Uint: return 1;
            case vk::Format::eR8Sint: return 1;
            case vk::Format::eR8Srgb: return 1;
            case vk::Format::eR8G8Unorm: return 2;
            case vk::Format::eR8G8Snorm: return 2;
            case vk::Format::eR8G8Uscaled: return 2;
            case vk::Format::eR8G8Sscaled: return 2;
            case vk::Format::eR8G8Uint: return 2;
            case vk::Format::eR8G8Sint: return 2;
            case vk::Format::eR8G8Srgb: return 2;
            case vk::Format::eR8G8B8Unorm: return 3;
            case vk::Format::eR8G8B8Snorm: return 3;
            case vk::Format::eR8G8B8Uscaled: return 3;
            case vk::Format::eR8G8B8Sscaled: return 3;
            case vk::Format::eR8G8B8Uint: return 3;
            case vk::Format::eR8G8B8Sint: return 3;
            case vk::Format::eR8G8B8Srgb: return 3;
            case vk::Format::eB8G8R8Unorm: return 3;
            case vk::Format::eB8G8R8Snorm: return 3;
            case vk::Format::eB8G8R8Uscaled: return 3;
            case vk::Format::eB8G8R8Sscaled: return 3;
            case vk::Format::eB8G8R8Uint: return 3;
            case vk::Format::eB8G8R8Sint: return 3;
            case vk::Format::eB8G8R8Srgb: return 3;
            case vk::Format::eR8G8B8A8Unorm: return 4;
            case vk::Format::eR8G8B8A8Snorm: return 4;
            case vk::Format::eR8G8B8A8Uscaled: return 4;
            case vk::Format::eR8G8B8A8Sscaled: return 4;
            case vk::Format::eR8G8B8A8Uint: return 4;
            case vk::Format::eR8G8B8A8Sint: return 4;
            case vk::Format::eR8G8B8A8Srgb: return 4;
            case vk::Format::eB8G8R8A8Unorm: return 4;
            case vk::Format::eB8G8R8A8Snorm: return 4;
            case vk::Format::eB8G8R8A8Uscaled: return 4;
            case vk::Format::eB8G8R8A8Sscaled: return 4;
            case vk::Format::eB8G8R8A8Uint: return 4;
            case vk::Format::eB8G8R8A8Sint: return 4;
            case vk::Format::eB8G8R8A8Srgb: return 4;
            case vk::Format::eA8B8G8R8UnormPack32: return 4;
            case vk::Format::eA8B8G8R8SnormPack32: return 4;
            case vk::Format::eA8B8G8R8UscaledPack32: return 4;
            case vk::Format::eA8B8G8R8SscaledPack32: return 4;
            case vk::Format::eA8B8G8R8UintPack32: return 4;
            case vk::Format::eA8B8G8R8SintPack32: return 4;
            case vk::Format::eA8B8G8R8SrgbPack32: return 4;
            case vk::Format::eA2R10G10B10UnormPack32: return 4;
            case vk::Format::eA2R10G10B10SnormPack32: return 4;
            case vk::Format::eA2R10G10B10UscaledPack32: return 4;
            case vk::Format::eA2R10G10B10SscaledPack32: return 4;
            case vk::Format::eA2R10G10B10UintPack32: return 4;
            case vk::Format::eA2R10G10B10SintPack32: return 4;
            case vk::Format::eA2B10G10R10UnormPack32: return 4;
            case vk::Format::eA2B10G10R10SnormPack32: return 4;
            case vk::Format::eA2B10G10R10UscaledPack32: return 4;
            case vk::Format::eA2B10G10R10SscaledPack32: return 4;
            case vk::Format::eA2B10G10R10UintPack32: return 4;
            case vk::Format::eA2B10G10R10SintPack32: return 4;
            case vk::Format::eR16Unorm: return 2;
            case vk::Format::eR16Snorm: return 2;
            case vk::Format::eR16Uscaled: return 2;
            case vk::Format::eR16Sscaled: return 2;
            case vk::Format::eR16Uint: return 2;
            case vk::Format::eR16Sint: return 2;
            case vk::Format::eR16Sfloat: return 2;
            case vk::Format::eR16G16Unorm: return 4;
            case vk::Format::eR16G16Snorm: return 4;
            case vk::Format::eR16G16Uscaled: return 4;
            case vk::Format::eR16G16Sscaled: return 4;
            case vk::Format::eR16G16Uint: return 4;
            case vk::Format::eR16G16Sint: return 4;
            case vk::Format::eR16G16Sfloat: return 4;
            case vk::Format::eR16G16B16Unorm: return 6;
            case vk::Format::eR16G16B16Snorm: return 6;
            case vk::Format::eR16G16B16Uscaled: return 6;
            case vk::Format::eR16G16B16Sscaled: return 6;
            case vk::Format::eR16G16B16Uint: return 6;
            case vk::Format::eR16G16B16Sint: return 6;
            case vk::Format::eR16G16B16Sfloat: return 6;
            case vk::Format::eR16G16B16A16Unorm: return 8;
            case vk::Format::eR16G16B16A16Snorm: return 8;
            case vk::Format::eR16G16B16A16Uscaled: return 8;
            case vk::Format::eR16G16B16A16Sscaled: return 8;
            case vk::Format::eR16G16B16A16Uint: return 8;
            case vk::Format::eR16G16B16A16Sint: return 8;
            case vk::Format::eR16G16B16A16Sfloat: return 8;
            case vk::Format::eR32Uint: return 4;
            case vk::Format::eR32Sint: return 4;
            case vk::Format::eR32Sfloat: return 4;
            case vk::Format::eR32G32Uint: return 8;
            case vk::Format::eR32G32Sint: return 8;
            case vk::Format::eR32G32Sfloat: return 8;
            case vk::Format::eR32G32B32Uint: return 12;
            case vk::Format::eR32G32B32Sint: return 12;
            case vk::Format::eR32G32B32Sfloat: return 12;
            case vk::Format::eR32G32B32A32Uint: return 16;
            case vk::Format::eR32G32B32A32Sint: return 16;
            case vk::Format::eR32G32B32A32Sfloat: return 16;
            case vk::Format::eR64Uint: return 8;
            case vk::Format::eR64Sint: return 8;
            case vk::Format::eR64Sfloat: return 8;
            case vk::Format::eR64G64Uint: return 16;
            case vk::Format::eR64G64Sint: return 16;
            case vk::Format::eR64G64Sfloat: return 16;
            case vk::Format::eR64G64B64Uint: return 24;
            case vk::Format::eR64G64B64Sint: return 24;
            case vk::Format::eR64G64B64Sfloat: return 24;
            case vk::Format::eR64G64B64A64Uint: return 32;
            case vk::Format::eR64G64B64A64Sint: return 32;
            case vk::Format::eR64G64B64A64Sfloat: return 32;
            case vk::Format::eB10G11R11UfloatPack32: return 4;
            case vk::Format::eE5B9G9R9UfloatPack32: return 4;
            default: throw std::runtime_error("Unimplemented vulkan format!");
        }
    } 
};