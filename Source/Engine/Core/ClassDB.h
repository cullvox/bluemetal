#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>
#include <string_view>
#include <span>

#include "Core/Print.h"

namespace bl
{

using namespace std;

class Object;
class Property;

using ObjectInstantiationFunc = Object* (* )();

class ClassData
{
    friend class ClassDB;

    std::string_view name = "";
    std::string_view parentClassName = "";
    ObjectInstantiationFunc instantiationFunc = nullptr;
    std::vector<std::unique_ptr<Property>> properties = {};
    std::vector<Property*> userPropertyPointers = {};
    std::unordered_map<std::string_view, std::size_t> nameToPropertyIndex = {};

public:
    ClassData() = default;
    ClassData(const ClassData&) = delete;
    ClassData(ClassData&&) = default;

    ClassData& operator=(const ClassData&) = delete;
    ClassData& operator=(ClassData&&) = default;

    /** @brief Returns the name of the class represented here. */
    std::string_view GetClassName() const;

    /** @brief Returns the name of the parent class. */
    std::string_view GetParentClassName() const;

    /** @brief Returns the class data for this objects parent. */
    const ClassData* GetParentClass() const;

    /** @brief Returns all the properties that this represented class contains, not any parent or child classes. */
    const std::span<Property* const> GetProperties() const;

    /** @brief Finds a property in the heirarchy from the root Object class to whatever class this may represent. */
    Property* FindPropertyInHeirarchy() const;

    /** @brief Instantiates an object of this class type. */
    Object* Instantiate() const;

    /** @brief Determines if a class can be cast down to another type. */
    bool IsChildOf(std::string_view name) const;

    /**
     * @brief Iterate through all properties on a class.
     */
    void ForEachProperty(std::function<void(Property*)> propertyIterator) const;

};

template<typename T>
concept EnumType = requires {
    std::is_integral_v<T>;
    std::is_same_v<std::underlying_type_t<T>, int64_t>;
};

struct EnumData
{
    friend class ClassDB;

    EnumData() = default;
    EnumData(const EnumData&) = delete;
    EnumData(EnumData&&) = default;
    EnumData& operator=(const EnumData&) = delete;
    EnumData& operator=(EnumData&&) = default;

    string_view name;
    uint32_t typeHash;
    unordered_map<string_view, int64_t> nameToValue;
    unordered_map<int64_t, string_view> valueToName;
    vector<pair<string_view, int64_t>> values;

public:
    /** @brief Returns the name of the this represented enum. */
    string_view GetEnumName() const;

    /** @brief Returns all the name value pairs. */
    span<pair<string_view, int64_t>> GetEnumValues() const;

    template<EnumType T>
    span<pair<string_view, T>> GetEnumValues() const {
        if (typeid(T).hash_code() != typeHash) {
            throw std::runtime_error("Invalid enum type operating on enum data!");
        }

        auto values = GetEnumValues();
        return span(reinterpret_cast<pair<string_view, T>*>(values.data()), values.size());
    }

    /** @brief Returns the name of a specific value. */
    string_view GetEnumValueName(int64_t value) const;

    template<EnumType T>
    string_view GetEnumValueName(T value) const {
        if (typeid(T).hash_code() != typeHash) {
            throw std::runtime_error("Invalid enum type operating on enum data!");
        }

        
    }

};

/** @brief A database containing information about all the classes used in this engine. */
class ClassDB
{
    ClassDB();
    ~ClassDB();

    vector<ClassData> _classes;
    vector<string_view> _classNames;
    unordered_map<string_view, std::size_t> _nameToClassIndex;

    vector<EnumData> _enums;
    vector<string_view> _enumNames;
    unordered_map<string_view, std::size_t> _nameToEnumIndex;

public:

    static ClassDB* Get();

    Object* Instantiate(string_view className);

    template<typename T>
    void Register()
    {
        T::RegisterClass();
    }

    template<EnumType T>
    void RegisterEnum(std::string_view enumName, const vector<pair<string_view, T>>& values)
    {

        // Check if the class already exists.
        if (_nameToEnumIndex.contains(enumName)) {
            Print::Error("Could not register enum, \"{}\", as it already exists.", enumName);
            return;
        }

        // Create the class data and begin populating.
        EnumData data;
        data.name = enumName;
        data.typeHash = typeid(T).hash_code();
        data.values.reserve(values.size());

        // Build the name map and reverse map.
        for (const auto& value : values) {
            data.nameToValue.emplace(value.first, static_cast<int64_t>(value.second));
            data.valueToName.emplace(static_cast<int64_t>(value.second), value.first);
            data.values.push_back(std::make_pair(value.first, static_cast<int64_t>(value.second)));
        }

        _enums.emplace_back(std::move(data));

        // Set the mapped name to the index.
        _nameToEnumIndex[enumName] = _enums.size() - 1;

        _enumNames.push_back(enumName);
    }

    void RegisterClass(std::string_view className, std::string_view parentClassName, ObjectInstantiationFunc instantiationFunc);
    void RegisterVirtualClass(std::string_view className, std::string_view parentClassName);
    void RegisterProperty(std::string_view className, std::unique_ptr<Property> property);

    const ClassData* FindClass(std::string_view name);
    const EnumData* FindEnum(std::string_view name);

    std::span<std::pair<std::string_view, int64_t>> GetEnumValues(std::string_view enumName);
    std::string_view GetEnumValueName(std::string_view enumName, int64_t value);

    

    bool HasClass(std::string_view name);
    std::span<const std::string_view> GetClassNames() const;
    std::span<Property*> GetClassProperties(const std::string_view name);

    Property* FindPropertyInClassRecursive(std::string_view name, std::string_view property);

    std::string_view GetClassParent(const std::string_view className);

    bool IsEnumValid(const std::string_view enumType, int64_t value);
};


} // namespace bl