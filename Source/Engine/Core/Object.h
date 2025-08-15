#pragma once

#include "Precompiled.h"
#include "Concepts.h"
#include "ObjectClasses.h"
#include "ObjectMacros.h"

#include "Property.h"

namespace bl {

class Object {
    static Object* _Creator();
    static void BindProperties();

    
    static std::unordered_map<std::string_view, Property> _properties;

public:
    virtual ~Object() = default;

    /**
     * 
     */
    virtual void PreInit() {};
    virtual void PostInit() {};
    virtual bool IsInitialized() {};
    virtual void NotifyChange() {};
    void Populate();

    void Set(std::string_view name, std::any value);
    std::any Get(std::string_view name);
};

}