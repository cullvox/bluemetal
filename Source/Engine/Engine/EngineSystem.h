#pragma once

#include "Core/NonCopyable.h"

namespace bl
{

class Engine;

class EngineSystem : public NonCopyable
{
public:
    EngineSystem(Engine* engine);
    ~EngineSystem();

protected:
    Engine* _engine;
};

} // namespace bl