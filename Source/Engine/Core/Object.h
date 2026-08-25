#pragma once

#include "Core/MacroUtils.h"
#include "Core/Variant.h"
#include <type_traits>

#define OBJECT_BOILER(name, parent) \
public: \
    virtual std::string_view GetClassName() override { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetStaticClassName() { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
    static Object* Create() { return new name(); } \
    virtual name* Clone() override { return new name(*this); } \
private:

#define OBJECT_BOILER_VIRTUAL(name, parent) \
public: \
    virtual std::string_view GetClassName() override { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetStaticClassName() { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
    virtual name* Clone() override { throw std::runtime_error("Cannot clone a virtual class that hasn't been overriden."); } \
    static Object* Create() { throw std::runtime_error("Cannot create a virtual object."); } \
private:

namespace bl
{
class ClassDB;
class Property;

class Object {
public:
    virtual std::string_view GetClassName() { return "Object"; }
    constexpr static std::string_view GetStaticClassName() { return "Object"; }
    constexpr static std::string_view GetParentClassName() { return ""; }
    static Object* Create() { return new Object(); }
    virtual Object* Clone() { return new Object(*this); }
private:
    std::vector<std::unique_ptr<Property>> _instanceProperties;
    std::vector<Property*> _instancePropertiesPointers;
    std::unordered_map<std::string_view, std::size_t> _nameToPropertyIndex;

protected:
    
    void AddInstanceProperty(std::unique_ptr<Property> property);

public:
    Object();
    Object(const Object& other);
    virtual ~Object();

    Object& operator=(const Object& other);
    Object& operator=(Object&& other);

    static void RegisterClass();

    /** @brief Returns true if this class is derived from a different class. */
    bool IsA(std::string_view className);

    template<typename T>
    T* As() {
        static_assert(std::is_base_of_v<Object, T>, "Must be a base of object.");
        return dynamic_cast<T*>(this);
    }

    std::span<Property*> GetInstanceProperties();

    /** @brief Returns an engine property value. */
    Variant Get(std::string_view name);

    /** @brief Sets an object property value. */
    void Set(std::string_view name, const Variant& value);

    /** @brief Returns the static engine device, same as Engine::Get(), just a little easier to read. */
    Engine* GetEngine();
};

} // namespace bl