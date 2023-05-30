#pragma once

#include "Noodle/Precompiled.hpp"
#include "Noodle/Export.h"

enum class blNoodleType
{
    eGroup,
    eInteger,
    eFloat,
    eBoolean,
    eString,
    eArray,
};

class BLUEMETAL_NOODLE_API blNoodle
{
public:
    static blNoodle parse(const std::string& noodle); // Use this to parse some noodle
    static blNoodle parseFromFile(const std::filesystem::path& path); // Use this to parse some noodle from a file

    blNoodle() = default; // Creates a default noodle of group type with no name
    blNoodle(const std::string& groupName, blNoodle* parent = nullptr); // Creates a group noodle
    blNoodle(const std::string& name, int value, blNoodle* parent = nullptr); // Creates a int noodle
    blNoodle(const std::string& name, float value, blNoodle* parent = nullptr); // Creates a float noodle
    blNoodle(const std::string& name, bool value, blNoodle* parent = nullptr); // Creates a bool noodle
    blNoodle(const std::string& name, const std::string& value, blNoodle* parent = nullptr); // Creates a string noodle
    
    template<typename T> 
    blNoodle(const std::string& name, const std::vector<T>& value, blNoodle* parent = nullptr); // Creates an array noodle

    blNoodleType type() const noexcept { return _type; } // Gets the type of this noodle
    blNoodleType arrayType() const; // If this noodle is an array it returns the type of the values
    size_t size() const; // Gets the amount of noodles or values this noodle contains
    std::string dump() const noexcept; // Recursively dumps the noodles into a human readable soup
    void dumpToFile(const std::filesystem::path& path) const noexcept; // Dumps the noodles into a file
    blNoodle* parent() const noexcept { return _parent; } // Returns the noodle one up the chains 
    bool isRoot() const noexcept { return _parent == nullptr; } // Returns true if this is the root noodle
    std::string toString() const noexcept; // Converts this noodle to a string value

    template<typename T> 
    T& get(); // Returns the value of this noodle, will except if invalid

    blNoodle& operator=(int value) noexcept; // Sets the noodle to a int
    blNoodle& operator=(float value) noexcept; // Sets the noodle to a float
    blNoodle& operator=(bool value) noexcept; // Sets the noodle to a bool
    blNoodle& operator=(const std::string& value) noexcept; // Sets the noodle to an string

    template<typename T> 
    blNoodle& operator=(const std::vector<T>& value) noexcept; // Sets the noodle to an array of values

    blNoodle& operator[](const std::string& key); // Returns the child noodle, inserts new
    blNoodle& operator[](size_t index); // Returns an indexed noodle of an array, will throw if not an array

    friend std::ostream& operator<<(std::ostream& os, const blNoodle& noodle);

private:
    void recursiveDump(std::stringstream& ss, int tabs) const noexcept;

    blNoodleType _type;
    std::string _name;
    blNoodle* _parent;
    std::map<std::string, blNoodle> _groupChildren;
    blNoodleType _arrayType;
    std::vector<blNoodle> _array;
    std::variant<int, float, bool, std::string> _value;
};

//// Template

#include "NoodleExceptions.hpp"

namespace ntu
{
    template <typename T, typename... U>
    struct is_any : std::disjunction<std::is_same<T, U>...> {};
}

template<typename T> 
blNoodle::blNoodle(const std::string& name, const std::vector<T>& value, blNoodle* parent)
    : _name(name)
    , _type(blNoodleType::eArray)
    , _parent(parent)
{


    static_assert(ntu::is_any<T, int, float, bool>() && "Must be one of the types!");

    for (const T& v : value)
    {
        _array.push_back(blNoodle("", v, parent));
    }    
}

template<typename T> 
T& blNoodle::get()
{
    if (_type != blNoodleType::eInteger &&
        _type != blNoodleType::eFloat &&
        _type != blNoodleType::eBoolean &&
        _type != blNoodleType::eString)
    {
        throw blNoodleInvalidAccessException("get expected integer, float, boolean, or string type");
    }

    static_assert(ntu::is_any<T, int, float, bool, std::string>() && "Must be one of the types!");
    return std::get<T>(_value);
}

template<typename T> 
blNoodle& blNoodle::operator=(const std::vector<T>& value) noexcept
{
    
    return *this;
}
