#pragma once

#include "Core/MacroUtils.h"
#include "Core/Variant.h"

#define OBJECT_BOILER(name, parent) \
public: \
    virtual std::string_view GetClassName() override { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetStaticClassName() { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
    static Object* Create(Engine& engine) { return new name(engine); } \
    virtual name* Clone() override { return new name(*this); } \
private:

#define OBJECT_BOILER_VIRTUAL(name, parent) \
public: \
    virtual std::string_view GetClassName() override { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetStaticClassName() { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
    static Object* Create(Engine& engine) { return nullptr; } \
private:

namespace bl
{
class Engine;
class ClassDB;
class Property;

class Object {
public:
    virtual std::string_view GetClassName() { return "Object"; }
    constexpr static std::string_view GetStaticClassName() { return "Object"; }
    constexpr static std::string_view GetParentClassName() { return ""; }
    static Object* Create(Engine& engine) { return new Object(engine); }
    virtual Object* Clone() { return new Object(*this); }
private:
    Engine& _engine;
    std::vector<std::unique_ptr<Property>> _instanceProperties;
    std::vector<Property*> _instancePropertiesPointers;
    std::unordered_map<std::string_view, std::size_t> _nameToPropertyIndex;

protected:
    
    void AddInstanceProperty(std::unique_ptr<Property> property);

public:
    Object(Engine& engine);
    Object(const Object& rhs);
    virtual ~Object();

    static void RegisterClass(ClassDB& db);
    bool IsA(std::string_view className);

    std::span<Property*> GetInstanceProperties();

    Variant Get(std::string_view name);
    void Set(std::string_view name, const Variant& value);

    Engine& GetEngine();
};

} // namespace bl