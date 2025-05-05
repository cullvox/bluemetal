#pragma once

#include "Precompiled.h"

namespace bl
{

class VulkanReflectedBlock;

enum class VulkanVariableBlockType
{
    eScalarBool,
    eScalarInt,
    eScalarFloat,
    eVector2,
    eVector3,
    eVector4,
    eMatrix4,
};

class VulkanVariableBlock
{
public:
    VulkanVariableBlock& SetBinding(uint32_t binding);
    VulkanVariableBlock& SetType(VulkanVariableBlockType type);
    VulkanVariableBlock& SetOffset(uint32_t offset);
    VulkanVariableBlock& SetSize(uint32_t size);
    VulkanVariableBlock& SetName(const std::string& name);

    uint32_t GetBinding() const;
    VulkanVariableBlockType GetType() const;
    uint32_t GetOffset() const;
    uint32_t GetSize() const;
    std::string GetName() const;
private:
    std::string _name;
    uint32_t _binding;
    VulkanVariableBlockType _type; /** @brief  */
    uint32_t _offset; /** @brief Offset of  */
    uint32_t _size; /** @brief Size of structure in bytes. */
};

class VulkanReflectedBlock
{
public:
    VulkanVariableBlock& operator[](const std::string& name);

    std::vector<VulkanVariableBlock> GetMembers() const;
    std::string GetName() const;
    uint32_t GetSize() const;

    VulkanReflectedBlock& SetName(const std::string& name);
    VulkanReflectedBlock& SetSize(uint32_t size);
private:
    std::string _name;
    uint32_t _size;

    /** @brief A variable in a buffer of some kind defined by a structure.
     * 
     * While in GLSL structures cannot be defined within structs they can be used
     * inside of themselves. In order to support this behaviour when looking for a
     * block from a variable from a structure within a struct use a dot!
     * 
     * For example:
     * 
     * ```GLSL
     * struct LightData
     * {
     *      float position;
     *      float viscosity;
     * };
     * 
     * layout(...) uniform struct Material
     * {
     *      float variable;
     *      float reflections;
     *      LightData light;
     * } mat;
     * ```
     * 
     * Accessing position within the LightData struct within the material struct
     * would look like this: "mat.light.position"
     * 
     */
    std::unordered_map<std::string, VulkanVariableBlock> _variables;
};

} // namespace bl