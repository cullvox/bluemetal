#include "Object.h"

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

Engine& Object::GetEngine()
{
    return _engine;
}

} // namespace bl