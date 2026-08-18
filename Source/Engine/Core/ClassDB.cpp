#include "ClassDB.h"
#include "Core/Reflection/Property.h"

namespace bl
{

ClassDB::ClassDB()
{
}

ClassDB::~ClassDB()
{
}

ClassDB* ClassDB::Get()
{
    static ClassDB db;
    return &db;
}

Object* ClassDB::Instantiate(const std::string_view className)
{
    // Find the class and check if it exists.
    auto it = _nameToClassIndex.find(className);

    if (it == _nameToClassIndex.end()) {
        Print::Error("Could not instantiate an invalid class, \"{}\".", className);
        return nullptr;
    }

    // Instantiate the new object.
    const std::size_t index = it->second;
    return _classes[index].instantiationFunc();
}

void ClassDB::RegisterClass(const std::string_view className, std::string_view parentClassName, ObjectInstantiationFunc instantiationFunc)
{
    // Check if the class already exists.
    if (_nameToClassIndex.contains(className)) {
        Print::Error("Could not register class, \"{}\", as it already exists.", className);
        return;
    }

    // Find parent class name.
    auto parentClassIt = _nameToClassIndex.find(parentClassName);
    if (parentClassIt == _nameToClassIndex.end() && !parentClassName.empty()) {
        throw std::runtime_error("Invalid parent class registered.");
    }

    // Create the class data and begin populating.
    ClassData data;
    data.name = className;
    data.parentClassName = parentClassName;
    data.instantiationFunc = instantiationFunc;

    _classes.emplace_back(std::move(data));

    // Set the mapped name to the index.
    _nameToClassIndex[className] = _classes.size() - 1;

    _classNames.push_back(className);
}

void ClassDB::RegisterVirtualClass(std::string_view className, std::string_view parentClassName)
{
    // Check if the class already exists.
    if (_nameToClassIndex.contains(className)) {
        Print::Error("Could not register virtual class, \"{}\", as it already exists.", className);
        return;
    }

    // Create the class data and begin populating.
    ClassData data;
    data.name = className;
    data.parentClassName = parentClassName;
    data.instantiationFunc = nullptr;

    _classes.emplace_back(std::move(data));

    // Set the mapped name to the index.
    _nameToClassIndex[className] = _classes.size() - 1;

    _classNames.push_back(className);
}

void ClassDB::RegisterProperty(const std::string_view className, std::unique_ptr<Property> property)
{
    // Find the class data and check if it exists.
    auto it = _nameToClassIndex.find(className);

    if (it == _nameToClassIndex.end()) {
        Print::Error("Could not register property to an invalid class, \"{}\".", className);
        return;
    }

    const std::size_t index = it->second;
    ClassData& classData = _classes[index];

    // Ensure that there are no properties with this name that already exist.
    if (classData.nameToPropertyIndex.contains(property->GetName())) {
        Print::Error("Could not register class property when it's name already exists, \"{}\".", property->GetName());
        return;
    }

    // Move the property ownership into our class data.
    classData.properties.push_back(std::move(property));

    // Store the user pointer.
    classData.userPropertyPointers.push_back(classData.properties.back().get());

    // Add property name to map.
    classData.nameToPropertyIndex[classData.properties.back()->GetName()] = classData.properties.size() - 1;
}


std::span<Property*> ClassDB::GetClassProperties(const std::string_view className)
{
        // Find the class data and check if it exists.
    auto it = _nameToClassIndex.find(className);

    if (it == _nameToClassIndex.end()) {
        Print::Error("Could get properties of an invalid class, \"{}\".", className);
        return {};
    }

    const std::size_t index = it->second;
    ClassData& classData = _classes[index];

    return classData.userPropertyPointers;
}


std::string_view ClassDB::GetClassParent(const std::string_view className)
{
    // Find the class data and check if it exists.
    auto it = _nameToClassIndex.find(className);

    if (it == _nameToClassIndex.end()) {
        Print::Error("Could get parent of an invalid class, \"{}\".", className);
        return {};
    }

    const std::size_t index = it->second;
    ClassData& classData = _classes[index];

    return classData.parentClassName;
}

const ClassData* ClassDB::FindClass(std::string_view name)
{
    auto it = _nameToClassIndex.find(name);
    if (it == _nameToClassIndex.end()) {
        return nullptr;
    }
    return &_classes[it->second];
}

std::span<std::pair<std::string_view, int64_t>> ClassDB::GetEnumValues(std::string_view enumName)
{
    // Find the class data and check if it exists.
    auto it = _nameToEnumIndex.find(enumName);

    if (it == _nameToEnumIndex.end()) {
        Print::Error("Could not get enum values for an invalid enum, \"{}\".", enumName);
        return {};
    }

    return _enums[it->second].values;
}

std::string_view ClassDB::GetEnumValueName(std::string_view enumName, int64_t value)
{
    // Find the class data and check if it exists.
    auto it = _nameToEnumIndex.find(enumName);

    if (it == _nameToEnumIndex.end()) {
        Print::Error("Could not get enum values for an invalid enum, \"{}\".", enumName);
        return {};
    }

    return _enums[it->second].valueToName[value];
}

std::span<const std::string_view> ClassDB::GetClassNames() const
{
    return std::span{_classNames};
}

bool ClassDB::IsEnumValid(std::string_view name, int64_t type)
{
    const auto indexIt = _nameToEnumIndex.find(name);
    if (indexIt == _nameToEnumIndex.end()) {
        Print::Error("Invalid enum type name: \"{}\"!", name);
        return false;
    }

    const std::size_t index = indexIt->second;
    const EnumData& enumData = _enums[index];

    return enumData.valueToName.contains(type);
}


Property* ClassDB::FindPropertyInClassRecursive(std::string_view name, std::string_view property)
{
    std::string_view className = name;
    while (!className.empty()) {
        if (className == "Object") {
            // Stop at Object, since it's the base class for all objects and we don't want to show its properties.
            break;
        }

        auto it = _nameToClassIndex.find(className);
        if (it == _nameToClassIndex.end()) break;

        auto& classData = _classes[it->second];

        auto propertyIt = classData.nameToPropertyIndex.find(property);
        if (propertyIt != classData.nameToPropertyIndex.end()) {
            return classData.properties[propertyIt->second].get();
        };

        className = GetClassParent(className);
    }

    return nullptr;
}

std::string_view ClassData::GetClassName() const
{
    return name;
}

std::string_view ClassData::GetParentClassName() const
{
    return parentClassName;
}

const ClassData* ClassData::GetParentClass() const
{
    return ClassDB::Get()->FindClass(parentClassName);
}

const std::span<Property* const> ClassData::GetProperties() const
{
    return std::span{userPropertyPointers.begin(), userPropertyPointers.end()};
}

Property* ClassData::FindPropertyInHeirarchy() const
{
    return nullptr;
}

bool ClassData::IsChildOf(std::string_view name) const
{
    const ClassData* parent = GetParentClass();
    while (parent) {
        if (parent->GetClassName() == name)
            return true;

        parent = parent->GetParentClass();
    }

    return false;
}

Object* ClassData::Instantiate() const
{
    return instantiationFunc();
}

}