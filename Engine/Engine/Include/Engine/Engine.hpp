#pragma once

#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

namespace bl
{

/** \brief Puts the engine together, runs the engine until exit.
* 
* This class is a very generalized ensuring that the engine is created
* properly and continues to run unless there is a quit request or a failure
* occurs. This initializes all parts of the engine including but not limited to
* rendering, input, windows, and resources. It works on a large scale and will 
* collapse the engine on a fatal error that cannot be recovered from.
* 
* \since BloodLust 1.0.0
* 
*/
class Engine
{

public:

    /** \brief Initializes the entire engine and loads libraries like SDL.
    *
    * The beginning of the engines initialization process starts here and this
    * constructor creates all the necessities of later engine processes for
    * rendering and input processing. Most errors occuring at this stage are
    * fatal.
    * 
    * In order to see if the engine was initialized successfully you must use
    * \ref good to check.
    * 
    * After the engine has successfully initialzed you can retrieve objects 
    * from this class required for rendering and other important operations.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa ~Engine
    * \sa good
    * 
    */
    Engine(const std::string& applicationName) noexcept;

    /** \brief Terminates the engine and its libraries.
    *
    * Destroys the entire engine from top down.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Engine
    * 
    */
    ~Engine() noexcept;

    /** \brief Checks the engine for successful initialization.
    * 
    * The engine can have errors at almost any stage of construction and 
    * runtime this function does a check of all components of the engine for 
    * proper initialization.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Engine()
    *
    */
    [[nodiscard]] bool good() const noexcept;

private:
    Window m_window;
    RenderDevice m_renderDevice;

};

} // namespace bl