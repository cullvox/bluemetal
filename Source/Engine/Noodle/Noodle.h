#pragma once

#include "Precompiled.h"
#include "Export.h"

/** @brief Different types of noodle that can be expressed. */
enum class NoodleType {
    eGroup,
    eInteger,
    eFloat,
    eBoolean,
    eString,
    eArray,
};

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

public:

    [[nodiscard]] bool isGroup() const noexcept;
    [[nodiscard]] bool isArray() const noexcept;
    [[nodiscard]] bool isInteger() const noexcept;
    [[nodiscard]] bool isFloat() const noexcept;
    [[nodiscard]] bool isBool() const noexcept;
    [[nodiscard]] bool isString() const noexcept;

    /** @brief Returns true if this is the root noodle. */
    bool isRoot() const noexcept { return _parent == nullptr; }

    /** @brief Gets the type of this noodle. */
    NoodleType getType() const noexcept { return _type; }

    /** @brief If this noodle is an array it returns the type of the values. */
    NoodleType getArrayType() const;

    /** @brief Gets the amount of noodles or values this noodle contains. */
    size_t getSize() const;

    /** @brief Returns the noodle one up the chains. */
    Noodle* getParent() { return _parent; }

    /** @brief Converts this noodle to a string value. */
    std::string toString() const;

    /** @brief Recursively dumps the noodles into a human readable soup. */
    std::string dump();

    /** @brief Dumps this noodle and it's children into a file. */
    void dumpToFile(const std::filesystem::path& path);

    /** @brief Returns the value of this noodle, will throw if invalid. */
    template<typename T>
    T& get();

    /** @brief Gets a noodle value of this type. */
    template<typename T>
    Noodle& operator=(T value);

    /** @brief Returns the child noodle, inserts new. */
    Noodle& operator[](const std::string& key);

    /** @brief Returns an indexed noodle of an array, will throw if not an array. */
    Noodle& operator[](size_t index);

    friend std::ostream& operator<<(std::ostream& os, const Noodle& noodle);

private:

    /** @brief Recursively dump to a string stream. */
    void recursiveDump(std::stringstream& ss, int tabs) const;

    Noodle*                                        _parent;
    NoodleType                                     _type;
    std::string                                    _name;
    std::variant<int, float, bool, std::string>    _value;
    std::map<std::string, Noodle>                  _groupChildren;
    NoodleType                                     _arrayType;
    std::variant<int*, float*, bool*, std::string*> _arr;


};

#include "Noodle.inl"
