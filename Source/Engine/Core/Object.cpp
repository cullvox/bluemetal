#include "Object.h"
#include "Core/ClassDB.h"

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

void Object::RegisterClass(ClassDB& db)
{
    db.RegisterClass("Object", "", &Object::Create);
}

} // namespace bl