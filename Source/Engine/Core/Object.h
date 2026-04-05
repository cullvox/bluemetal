#pragma once

#include "Core/MacroUtils.h"

#define OBJECT_BOILER(name, parent) \
public: \
    constexpr virtual std::string_view GetClassName() { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetStaticClassName() { return BL_STRINGIFY(name); } \
    constexpr static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
    static Object* Create(Engine& engine) { return new name(engine); } \
    virtual name* Clone() { return new name(*this); } \
private:

namespace bl
{
class Engine;
class ClassDB;

class Object {
    OBJECT_BOILER(Object, )

    Engine& _engine;
public:
    Object(Engine& engine);
    Object(const Object& rhs);
    virtual ~Object();

    static void RegisterClass(ClassDB& db);

    Engine& GetEngine();
};

} // namespace bl