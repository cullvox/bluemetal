#pragma once

namespace bl
{

/// @brief Ensures that the whole engine is put together, runs and exits.
class Engine
{

public:

    /// @brief Constructor initializes the engine and libraries.
    Engine();

    /// @brief Destructor terminates the engine and libraries.
    ~Engine();

    InputSystem& getInputSystem();

};

} // namespace bl