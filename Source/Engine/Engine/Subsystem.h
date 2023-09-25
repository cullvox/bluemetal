#pragma once

namespace bl
{

class Subsystem
{
public:
    Subsystem() = default;
    ~Subsystem() = default;

    virtual void init() = 0;
    virtual void shutdown() = 0;
};

} // namespace bl
