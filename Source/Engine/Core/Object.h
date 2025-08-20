#pragma once

#include "Precompiled.h"
#include "Concepts.h"
#include "ObjectClasses.h"
#include "ObjectMacros.h"

#include "Property.h"

namespace bl {

class Object {
    static Object* _Creator()
    {
        return new Object();
    }

    static void BindProperties()
    { // Objects have no properties by default.
    }

    static std::unordered_map<std::string_view, std::unique_ptr<PropertyBase>> _properties;

protected:

    template<ObjectType TClass, VariantType TProperty>
    void AddProperty(std::string_view name, TProperty (TClass::* getter)(), void (TClass::* setter)(TProperty))
    {
        _properties[name] = std::make_unique<PropertyValue<TClass, TProperty>>(name, getter,setter);
    }

    void RemoveProperty(std::string_view name)
    {
        _properties.erase(name);
    }

public:
    Object();

    virtual ~Object() = default;

    /**
     * 
     */
    virtual void PreInit() {};
    virtual void PostInit() {};
    virtual bool IsInitialized() {};
    virtual void NotifyChange() {};
    void Populate();

    Variant GetProperty(std::string_view name);
    void SetProperty(std::string_view name, const Variant& value);
};

}