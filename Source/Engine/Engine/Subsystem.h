#pragma once

namespace bl
{

class Subsystem
{
public:
    Subsystem(Engine& engine);
    ~Subsystem();

    virtual void init();
    virtual void shutdown();

public:

};

} // namespace bl