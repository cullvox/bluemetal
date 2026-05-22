#pragma once

#include <cstdint>
#include <vector>
#include <string_view>
#include <span>

#include "Object.h"
#include "Reflection/Property.h"

namespace bl
{

using ObjectInstantiationFunc = Object* (* )(Engine& engine);

class ClassDB
{
    Engine& _engine;

    struct ClassData
    {
        ClassData() = default;
        ClassData(const ClassData&) = delete;
        ClassData(ClassData&&) = default;
        ClassData& operator=(const ClassData&) = delete;
        ClassData& operator=(ClassData&&) = default;

        std::string_view name;
        std::string_view parentClassName;
        ObjectInstantiationFunc instantiationFunc;
        std::vector<std::unique_ptr<Property>> properties;
        std::vector<Property*> userPropertyPointers;
        std::unordered_map<std::string_view, std::size_t> nameToPropertyIndex;
    };

    std::vector<ClassData> _classes;
    std::vector<std::string_view> _classNames;
    std::unordered_map<std::string_view, std::size_t> _nameToClassIndex;

    struct EnumData
    {
        EnumData() = default;
        EnumData(const EnumData&) = delete;
        EnumData(EnumData&&) = default;
        EnumData& operator=(const EnumData&) = delete;
        EnumData& operator=(EnumData&&) = default;

        std::string_view name;
        std::unordered_map<std::string_view, int64_t> nameToValue;
        std::unordered_map<int64_t, std::string_view> valueToName;
        std::vector<std::pair<std::string_view, int64_t>> values;
    };

    std::vector<EnumData> _enums;
    std::vector<std::string_view> _enumNames;
    std::unordered_map<std::string_view, std::size_t> _nameToEnumIndex;

public:
    ClassDB(Engine& engine);
    ~ClassDB();

    Object* Instantiate(std::string_view className);

    template<typename T>
    void Register()
    {
        T::RegisterClass(*this);
    }

    template<typename T>
    void RegisterEnum(std::string_view enumName, const std::vector<std::pair<std::string_view, T>>& values)
    {
        static_assert(std::is_enum_v<T> == true && std::is_same_v<int64_t, std::underlying_type_t<T>> == true);

        // Check if the class already exists.
        if (_nameToEnumIndex.contains(enumName)) {
            Print::Error("Could not register enum, \"{}\", as it already exists.", enumName);
            return;
        }

        // Create the class data and begin populating.
        EnumData data;
        data.name = enumName;
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
    void RegisterProperty(std::string_view className, std::unique_ptr<Property> property);

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