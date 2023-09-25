#pragma once

#include "Precompiled.h"
#include "Export.h"

namespace bl {

/** @brief Different types of noodle that can be expressed. */
enum class NoodleType {
    eInteger,
    eFloat,
    eBoolean,
    eString,
    eGroup,
    eArray,
};

/** @brief Any value type that a noodle can be. */
using NoodleValue = std::variant<int, float, bool, std::string>;

/** @brief Any array type that a noodle can be. */
using NoodleArray = std::variant<std::vector<int>, std::vector<float>, std::vector<bool>, std::vector<std::string>>;

/** @brief One node of the noodle. */
class BLUEMETAL_API Noodle {
public:

    /** @brief Use this to parse some noodle */
    static Noodle parse(const std::string& noodle);

    /** @brief Use this to parse some noodle from a file */
    static Noodle parseFromFile(const std::filesystem::path& path);

public:

    /** @brief Creates a default noodle of group type with no name */
    Noodle() = default;

    /** @brief Creates a group noodle */
    Noodle(const std::string& groupName, Noodle* parent = nullptr);

    /** @brief Creates a int noodle */
    Noodle(const std::string& name, int value, Noodle* parent = nullptr);

    /** @brief Creates a float noodle */
    Noodle(const std::string& name, float value, Noodle* parent = nullptr);

    /** @brief Creates a bool noodle */
    Noodle(const std::string& name, bool value, Noodle* parent = nullptr);

    /** @brief Creates a string noodle */
    Noodle(const std::string& name, const std::string& value, Noodle* parent = nullptr);

    /** @brief Create a noodle with an array. */
    template<typename T>
    Noodle(const std::string& name, const std::vector<T>& value, Noodle* parent = nullptr);

    /** @brief Sets the noodle to a new value. */
    Noodle& operator=(int value) noexcept;
    Noodle& operator=(float value) noexcept;
    Noodle& operator=(bool value) noexcept;
    Noodle& operator=(const std::string& value) noexcept;

    /** @brief Returns a noodle of a group using a key. */
    Noodle& operator[](const std::string& key);
    const Noodle& operator[](const std::string& key) const;

public:

    /** @brief Returns true if the noodle is the specified type. */
    [[nodiscard]] bool isInteger() const noexcept;
    [[nodiscard]] bool isFloat() const noexcept;
    [[nodiscard]] bool isBool() const noexcept;
    [[nodiscard]] bool isString() const noexcept;
    [[nodiscard]] bool isGroup() const noexcept;
    [[nodiscard]] bool isArray() const noexcept;

    /** @brief Returns true if this is the root noodle. */
    [[nodiscard]] bool isRoot() const noexcept;

    /** @brief Returns the noodle one up the chains. */
    [[nodiscard]] Noodle* getParent() const noexcept;

    /** @brief Gets the type of this noodle. */
    [[nodiscard]] NoodleType getType() const noexcept;

    /** @brief Gets the amount of noodles or values this noodle may contains, 0 if not group/array. */
    [[nodiscard]] size_t getSize() const noexcept;

    /** @brief Sets the name of this noodle and changes this parent group. */
    bool setName(const std::string& name) noexcept;

    /** @brief Gets a value and returns true if it was found. */
    [[nodiscard]] bool get(int& value) const noexcept;
    [[nodiscard]] bool get(float& value) const noexcept;
    [[nodiscard]] bool get(bool& value) const noexcept;
    [[nodiscard]] bool get(std::string& value) const noexcept;

    [[nodiscard]] bool at(size_t index, int&& value) const noexcept;
    [[nodiscard]] bool at(size_t index, float&& value) const noexcept;
    [[nodiscard]] bool at(size_t index, bool&& value) const noexcept;
    [[nodiscard]] bool at(size_t index, std::string&& value) const noexcept;

    /** @brief Returns true if a group contains a value. */
    [[nodiscard]] bool contains(const std::string& key) const noexcept;

    /** @brief Converts this noodle's value into a string. */
    [[nodiscard]] std::string toString() const noexcept;

    /** @brief Recursively dumps the noodles into a human readable soup. */
    [[nodiscard]] std::string dump() const noexcept;

    /** @brief Dumps this noodle and it's children into a file. */
    [[nodiscard]] bool dumpToFile(const std::filesystem::path& path) const noexcept;

public: /* Throw Functions */

    /** @brief Returns a value at an array index, must be an array type. */
    template<typename T>
    T& at(size_t index);

    template<typename T>
    T at(size_t index) const;

    /** @brief Returns the value of this noodle. */
    [[nodiscard]] int getInt() const;
    [[nodiscard]] float getFloat() const;
    [[nodiscard]] bool getBool() const;
    [[nodiscard]] std::string getString() const;

    /** @brief If this noodle is an array it returns the type of the values. */
    [[nodiscard]] NoodleType getArrayType() const;

private:

    /** @brief Recursively dump into a string stream. */
    void recursiveDump(std::stringstream& ss, int tabs) const;

    bool processArray();

    Noodle*                                    _parent;
    NoodleType                                 _type;
    std::string                                _name;
    NoodleValue                                _value;
    std::unordered_map<std::string, Noodle>    _groupChildren;
    NoodleType                                 _arrayType;
    NoodleArray                                _array;
};

} // namespace bl

#include "Noodle.inl"
