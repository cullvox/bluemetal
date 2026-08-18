#include "Object.h"
#include "Engine/Engine.h"
#include "Core/ClassDB.h"
#include "Core/ClassDB.h"
#include "Reflection/Property.h"

namespace bl
{

Object::Object()
{
}

Object::Object(const Object& rhs)
{
}

Object::~Object()
{
}

std::span<Property*> Object::GetInstanceProperties()
{
    return _instancePropertiesPointers;
}

void Object::AddInstanceProperty(std::unique_ptr<Property> property)
{
    _instanceProperties.push_back(std::move(property));
    _nameToPropertyIndex[_instanceProperties.back()->GetName()] = _instanceProperties.size() - 1;
    _instancePropertiesPointers.push_back(_instanceProperties.back().get());
}

Engine* Object::GetEngine()
{
    return Engine::Get();
}

Variant Object::Get(std::string_view name)
{
    ClassDB* db = ClassDB::Get();

    // Look for the property in the class db.
    auto property = db->FindPropertyInClassRecursive(GetClassName(), name);
    if (property) 
        return property->Get(this);

    // Look through the instance properties.
    auto it = _nameToPropertyIndex.find(name);
    if (it == _nameToPropertyIndex.end()) {
        Print::Error("Invalid property name to get from object.");
        return NULL;
    }

    return _instanceProperties[it->second]->Get(this);
}

void Object::Set(std::string_view name, const Variant& value)
{

    // Look for the property in the class db.
    auto property = ClassDB::Get()->FindPropertyInClassRecursive(GetClassName(), name);
    if (property) 
        return property->Set(this, value);

    // Look through the instance properties.
    auto it = _nameToPropertyIndex.find(name);
    if (it == _nameToPropertyIndex.end()) {
        Print::Error("Invalid property name ({}) to set on object.", name);
        return;
    }

    return _instanceProperties[it->second]->Set(this, value);
}

bool Object::IsA(std::string_view className)
{
    ClassDB* db = ClassDB::Get();

    std::string_view currentClass = GetClassName();
    while (!currentClass.empty()) {
        if (currentClass == className) {
            return true;
        }

        currentClass = db->GetClassParent(currentClass);
    }

    return false;
}

void Object::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Object", "", &Object::Create);
}

} // namespace bl