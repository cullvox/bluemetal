#pragma once

#include "Precompiled.h"
#include "Concepts.h"

namespace bl {

class Object;

class ObjectClass {
    template<ObjectType T>
    friend class ObjectRegistration;

    std::string_view parent;
    std::vector<std::string_view> children;
    bool isVirtual; // Cannot be created.

    Object* (* creatorFunc)(); // Objects must basically be default constructable.

public:
};

class ObjectClasses {
    static std::unordered_map<std::string_view, ObjectClass> _objects;

protected:
    template<ObjectType TObject>
    friend class ObjectRegistration;

    static ObjectClass& AddObjectClass(std::string_view name);

public:
    static const ObjectClass* GetObjectClass(std::string_view name);
    static ObjectClass* GetParentClass(std::string_view name);
    static Object* CreateObject(std::string_view name);
};

template<ObjectType TObject>
class ObjectRegistration
{
public:
    ObjectRegistration()
    {
        std::string_view name = TObject::ClassName();
        std::string_view parentName = TObject::ParentClassName();
        ObjectClass& def = ObjectClasses::AddObjectClass(name);
        def.parent = parentName;
        def.creatorFunc = &TObject::_Creator;
    }
};

}