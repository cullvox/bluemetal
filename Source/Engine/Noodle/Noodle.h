#pragma once

#include "Precompiled.h"
#include "Export.h"

enum class blNoodleType
{
    eGroup,
    eInteger,
    eFloat,
    eBoolean,
    eString,
    eArray,
};

class BLUEMETAL_API blNoodle
{
public:
    static blNoodle parse(const std::string& noodle); // Use this to parse some noodle
    static blNoodle parseFromFile(const std::filesystem::path& path); // Use this to parse some noodle from a file

    explicit blNoodle() = default; // Creates a default noodle of group type with no name
    explicit blNoodle(const std::string& groupName, blNoodle* parent = nullptr); // Creates a group noodle
    explicit blNoodle(const std::string& name, int value, blNoodle* parent = nullptr); // Creates a int noodle
    explicit blNoodle(const std::string& name, float value, blNoodle* parent = nullptr); // Creates a float noodle
    explicit blNoodle(const std::string& name, bool value, blNoodle* parent = nullptr); // Creates a bool noodle
    explicit blNoodle(const std::string& name, const std::string& value, blNoodle* parent = nullptr); // Creates a string noodle
    
    template<typename T> 
    explicit blNoodle(const std::string& name, const std::vector<T>& value, blNoodle* parent = nullptr); // Creates an array noodle

    blNoodleType type() { return _type; } // Gets the type of this noodle
    blNoodleType arrayType(); // If this noodle is an array it returns the type of the values
    size_t size(); // Gets the amount of noodles or values this noodle contains
    std::string dump(); // Recursively dumps the noodles into a human readable soup
    void dumpToFile(const std::filesystem::path& path); // Dumps the noodles into a file
    blNoodle* parent() { return _parent; } // Returns the noodle one up the chains 
    bool isRoot() { return _parent == nullptr; } // Returns true if this is the root noodle
    std::string toString(); // Converts this noodle to a string value

    template<typename T> 
    T& get(); // Returns the value of this noodle, will except if invalid

    blNoodle& operator=(int value); // Sets the noodle to a int
    blNoodle& operator=(float value); // Sets the noodle to a float
    blNoodle& operator=(bool value); // Sets the noodle to a bool
    blNoodle& operator=(const std::string& value); // Sets the noodle to an string

    template<typename T> 
    blNoodle& operator=(const std::vector<T>& value); // Sets the noodle to an array of values

    blNoodle& operator[](const std::string& key); // Returns the child noodle, inserts new
    blNoodle& operator[](size_t index); // Returns an indexed noodle of an array, will throw if not an array

    friend std::ostream& operator<<(std::ostream& os, const blNoodle& noodle);

private:
    void recursiveDump(std::stringstream& ss, int tabs);

    blNoodleType                                _type;
    std::string                                 _name;
    blNoodle*                                   _parent;
    std::map<std::string, blNoodle>             _groupChildren;
    blNoodleType                                _arrayType;
    std::vector<blNoodle>                       _array;
    std::variant<int, float, bool, std::string> _value;
};

#include "Noodle.inl"