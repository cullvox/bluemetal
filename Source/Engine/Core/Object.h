#pragma once

#include "Core/MacroUtils.h"

#define OBJECT_BOILER(name, parent) \
public: \
    constexpr virtual std::string_view GetClassName() override { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetStaticClassName() { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
    static Object* Create(Engine& engine) { return new name(engine); } \
    virtual name* Clone() override { return new name(*this); } \
private:

namespace bl
{
class Engine;
class ClassDB;

class Object {
public:
    constexpr virtual std::string_view GetClassName() { return "Object"; }
    constexpr static std::string_view GetStaticClassName() { return "Object"; }
    constexpr static std::string_view GetParentClassName() { return ""; }
    static Object* Create(Engine& engine) { return new Object(engine); }
    virtual Object* Clone() { return new Object(*this); }
private:
    Engine& _engine;

public:
    Object(Engine& engine);
    Object(const Object& rhs);
    virtual ~Object();

    static void RegisterClass(ClassDB& db);

    Engine& GetEngine();
};

} // namespace bl