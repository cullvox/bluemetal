#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace bl {

/**
 * @enum VulkanVariableBlockType
 * @brief Types of variables that can be stored in a Vulkan variable block.
 *
 * This enum defines the types of variables that can be stored in a Vulkan variable block.
 */
enum class VulkanVariableBlockType {
    eScalarBool,
    eScalarInt,
    eScalarFloat,
    eVector2,
    eVector3,
    eVector4,
    eMatrix4,
};

/**
 * @class VulkanVariableBlock
 * @brief Represents a variable block in a Vulkan pipeline.
 *
 * This class holds information about a variable block, including its binding,
 * type, offset, size, and name. It is used for reflection and parameterization
 * of uniform blocks in Vulkan pipelines.
 */
class VulkanVariableBlock {
    uint32_t _binding;
    VulkanVariableBlockType _type;
    uint32_t _offset;
    uint32_t _size;
    std::string _name;
public:
    VulkanVariableBlock() = default;
    VulkanVariableBlock(uint32_t binding, VulkanVariableBlockType type, uint32_t offset, uint32_t size, const std::string& name)
        : _binding(binding)
        , _type(type)
        , _offset(offset)
        , _size(size)
        , _name(name)
    {
    }

    /**
     * @brief Sets the binding for this variable block.
     * @param binding The binding index for this variable block.
     * @return Reference to this variable block for chaining.
     */
    VulkanVariableBlock& SetBinding(uint32_t binding);

    /**
     * @brief Sets the type for this variable block.
     * @param type The type of the variable block.
     * @return Reference to this variable block for chaining.
     */
    VulkanVariableBlock& SetType(VulkanVariableBlockType type);

    /**
     * @brief Sets the offset for this variable block.
     * @param offset The offset in bytes of the variable block.
     * @return Reference to this variable block for chaining.
     */
    VulkanVariableBlock& SetOffset(uint32_t offset);

    /**
     * @brief Sets the size for this variable block.
     * @param size The size in bytes of the variable block.
     * @return Reference to this variable block for chaining.
     */
    VulkanVariableBlock& SetSize(uint32_t size);

    /**
     * @brief Sets the name for this variable block.
     * @param name The name of the variable block.
     * @return Reference to this variable block for chaining.
     */
    VulkanVariableBlock& SetName(const std::string& name);

    /**
     * @brief Gets the binding index of this variable block.
     * @return The binding index of this variable block.
     */
    uint32_t GetBinding() const;

    /**
     * @brief Gets the type of this variable block.
     * @return The type of this variable block.
     */
    VulkanVariableBlockType GetType() const;

    /**
     * @brief Gets the offset of this variable block.
     * @return The offset in bytes of this variable block.
     */
    uint32_t GetOffset() const;

    /**
     * @brief Gets the size of this variable block.
     * @return The size in bytes of this variable block.
     */
    uint32_t GetSize() const;

    /**
     * @brief Gets the name of this variable block.
     * @return The name of this variable block.
     */
    const std::string& GetName() const;
};

/**
 * @class VulkanReflectedBlock
 * @brief Represents a reflected block in a Vulkan pipeline.
 *
 * This class holds information about a reflected block, including its name,
 * size, and the variables it contains. It is used for reflection and parameterization
 * of uniform blocks in Vulkan pipelines.
 */
class VulkanReflectedBlock {
public:
    VulkanReflectedBlock() = default;
    VulkanReflectedBlock(const std::string& name, uint32_t size)
        : _name(name)
        , _size(size)
    {
    }

    /**
     * @brief Sets the name of this reflected block.
     * @param name The name of the reflected block.
     * @return Reference to this reflected block for chaining.
     */
    VulkanReflectedBlock& SetName(const std::string& name);

    /**
     * @brief Sets the size of this reflected block.
     * @param size The size in bytes of the reflected block.
     * @return Reference to this reflected block for chaining.
     */
    VulkanReflectedBlock& SetSize(uint32_t size);

    /**
     * @brief Gets the name of this reflected block.
     * @return The name of the reflected block.
     */
    std::string GetName() const;

    /**
     * @brief Gets the size of this reflected block.
     * @return The size in bytes of the reflected block.
     */
    uint32_t GetSize() const;

    /**
     * @brief Add or get a variable in this reflected block.
     * @param name The name of the variable to add or get.
     * @return Reference to the VulkanVariableBlock for the given name.
     */
    VulkanVariableBlock& operator[](const std::string& name);

    /**
     * @brief Gets all variables in this reflected block.
     * @return A vector of VulkanVariableBlocks.
     */
    std::vector<VulkanVariableBlock> GetMembers() const;

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