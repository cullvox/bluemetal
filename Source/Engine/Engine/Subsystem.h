#pragma once

namespace bl
{

class Subsystem
{
public:
    Subsystem() = default;
    ~Subsystem() = default;

    virtual void update() {}
};

} // namespace bl
