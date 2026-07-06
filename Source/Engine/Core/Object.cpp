#include "Object.h"
#include "Engine/Engine.h"
#include "Core/ClassDB.h"

namespace bl
{

Object::Object(Engine& engine)
    : _engine(engine)
{
}

Object::Object(const Object& rhs)
    : _engine(rhs._engine)
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

Engine& Object::GetEngine()
{
    return _engine;
}

Variant Object::Get(std::string_view name)
{
    ClassDB& db = GetEngine().GetClassDB();

    // Look for the property in the class db.
    auto property = db.FindPropertyInClassRecursive(GetClassName(), name);
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
    ClassDB& db = GetEngine().GetClassDB();

    // Look for the property in the class db.
    auto property = db.FindPropertyInClassRecursive(GetClassName(), name);
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
    ClassDB& db = GetEngine().GetClassDB();

    std::string_view currentClass = GetClassName();
    while (!currentClass.empty()) {
        if (currentClass == className) {
            return true;
        }

        currentClass = db.GetClassParent(currentClass);
    }

    return false;
}

void Object::RegisterClass(ClassDB& db)
{
    db.RegisterClass("Object", "", &Object::Create);
}

} // namespace bl