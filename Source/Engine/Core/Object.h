#pragma once

#include <string_view>


#define OBJECT_BOILER(name, parent) \
public: \
    static std::string_view GetClassName() { return BL_STRINGIFY(name); } \
    static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
    static Object* Create(Engine& engine) { return new name(engine); } \
    virtual name* Clone() { return new name(*this); } \
private:

namespace bl
{
class Engine;

class Object {
    OBJECT_BOILER(Object, )

    Engine& _engine;
public:
    Object(Engine& engine);
    Object(const Object& rhs);
    virtual ~Object();

    Engine& GetEngine();
};

} // namespace bl