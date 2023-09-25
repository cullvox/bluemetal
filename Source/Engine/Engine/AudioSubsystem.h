#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Math/Vector3.h"
#include "Audio/AudioSource.h"
#include "Audio/AudioListener.h"

namespace bl
{

///////////////////////////////
// Forward Declarations
///////////////////////////////

class Engine;
class AudioOutput;
class AudioInput;

///////////////////////////////
// Classes
///////////////////////////////

struct AudioSubsystemInitInfo
{
    uint32_t inputId;
    uint32_t outputId;
};

class AudioSubsystem
{
public:

    /// Engine Constructor
    AudioSubsystem(Engine* engine);

    /// Default Destructor
    ~AudioSubsystem();

    void init();
    void shutdown();
    void update();

    std::vector<AudioInput*> getInputs();
    std::vector<AudioOutput> getOutputs();
    std::unique_ptr<Sound> createSound(std::filesystem::path path);
    std::unique_ptr<AudioListener> createListener();
    std::unique_ptr<AudioSource> createSource();

private:
    Engine* engine;

    std::unique_ptr<AudioSystem> system;

};

} // namespace bl