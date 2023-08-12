#pragma once

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

class Creatable
{
public:
    Creatable() = default;
    Creatable(Creatable& rhs) = default;
    Creatable(Creatable&& rhs) = default;
    ~Creatable() = default;

    Creatable& operator=(const Creatable& rhs) = default;
    Creatable& operator=(Creatable&& rhs) = default;

    /// Creates this objects instance of it's purpose. 
    bool create();

    /// Destroys this objects instance of it's purpose.
    void destroy();
}

} // namespace bl