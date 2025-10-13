#pragma once

#include "Precompiled.h"
#include "Concepts.h"
#include "Object.h"

namespace bl {

class ObjectClass : public Object {
    OBJECT_CLASS(ObjectClass, Object)

    ObjectClass* _parent;
    std::list<ObjectClass*> _children;
    bool _isVirtual; // Cannot be created.
    Object* (* _instantiateFunc)(); // Objects must basically be default constructable.

public:
    std::string_view GetName();
    ObjectClass* GetParentClass(); // if type is 'Object', returns nullptr
    bool IsVirtual(); // returns true if the object cannot be instantiated
    bool IsDerivedFrom(std::string_view name); // looks up the parent heirarchy for derivative of the class by name.
    Object* Instantiate(); // returns nullptr if IsVirtual()
    // std::vector<PropertyBase*> GetProperties();
};

}