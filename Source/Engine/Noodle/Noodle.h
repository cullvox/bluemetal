#pragma once

#include "Precompiled.h"
#include "Export.h"

enum class NoodleType
{
    eGroup,
    eInteger,
    eFloat,
    eBoolean,
    eString,
    eArray,
};

class BLUEMETAL_API Noodle
{
public:
    static Noodle parse(const std::string& noodle); // Use this to parse some noodle
    static Noodle parseFromFile(const std::filesystem::path& path); // Use this to parse some noodle from a file

    explicit Noodle() = default; // Creates a default noodle of group type with no name
    explicit Noodle(const std::string& groupName, Noodle* parent = nullptr); // Creates a group noodle
    explicit Noodle(const std::string& name, int value, Noodle* parent = nullptr); // Creates a int noodle
    explicit Noodle(const std::string& name, float value, Noodle* parent = nullptr); // Creates a float noodle
    explicit Noodle(const std::string& name, bool value, Noodle* parent = nullptr); // Creates a bool noodle
    explicit Noodle(const std::string& name, const std::string& value, Noodle* parent = nullptr); // Creates a string noodle
    
    template<typename T> 
    explicit Noodle(const std::string& name, const std::vector<T>& value, Noodle* parent = nullptr); // Creates an array noodle

    NoodleType type() { return _type; } // Gets the type of this noodle
    NoodleType arrayType(); // If this noodle is an array it returns the type of the values
    size_t size(); // Gets the amount of noodles or values this noodle contains
    std::string dump(); // Recursively dumps the noodles into a human readable soup
    void dumpToFile(const std::filesystem::path& path); // Dumps the noodles into a file
    Noodle* parent() { return _parent; } // Returns the noodle one up the chains 
    bool isRoot() const { return _parent == nullptr; } // Returns true if this is the root noodle
    std::string toString() const; // Converts this noodle to a string value

    template<typename T> 
    T& get(); // Returns the value of this noodle, will except if invalid

    Noodle& operator=(int value); // Sets the noodle to a int
    Noodle& operator=(float value); // Sets the noodle to a float
    Noodle& operator=(bool value); // Sets the noodle to a bool
    Noodle& operator=(const std::string& value); // Sets the noodle to an string

    template<typename T> 
    Noodle& operator=(const std::vector<T>& value); // Sets the noodle to an array of values

    Noodle& operator[](const std::string& key); // Returns the child noodle, inserts new
    Noodle& operator[](size_t index); // Returns an indexed noodle of an array, will throw if not an array

    friend std::ostream& operator<<(std::ostream& os, const Noodle& noodle);

private:
    void recursiveDump(std::stringstream& ss, int tabs) const;

    NoodleType                                _type;
    std::string                                 _name;
    Noodle*                                   _parent;
    std::map<std::string, Noodle>             _groupChildren;
    NoodleType                                _arrayType;
    std::vector<Noodle>                       _array;
    std::variant<int, float, bool, std::string> _value;
};

#include "Noodle.inl"