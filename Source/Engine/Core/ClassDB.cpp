#include "ClassDB.h"

namespace bl
{

ClassDB::ClassDB(Engine& engine)
    : _engine(engine)
{
}

ClassDB::~ClassDB()
{
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
    return _classes[index].instantiationFunc(_engine);
}

void ClassDB::RegisterClass(const std::string_view className, ObjectInstantiationFunc instantiationFunc)
{
    // Check if the class already exists.
    if (_nameToClassIndex.contains(className)) {
        Print::Error("Could not register class, \"{}\", as it already exists.", className);
        return;
    }

    // Create the class data and begin populating.
    ClassData data;
    data.name = className;
    data.instantiationFunc = instantiationFunc;

    _classes.emplace_back(std::move(data));

    // Set the mapped name to the index.
    _nameToClassIndex[className] = _classes.size() - 1;
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
    classData.userPropertyPointers.push_back(classData.properties.end()->get());

    // Add property name to map.
    classData.nameToPropertyIndex[property->GetName()] = classData.properties.size() - 1;
}


const std::vector<Property*>& ClassDB::GetClassProperties(const std::string_view className)
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

}