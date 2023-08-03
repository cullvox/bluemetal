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
    uint32_t inputDeviceIndex;
    uint32_t outputDeviceIndex;
};

class AudioSubsystem
{
public:

    /// Engine Constructor
    AudioSubsystem(Engine& engine);

    /// Default Destructor
    ~AudioSubsystem();

    std::vector<std::shared_ptr<AudioInput>> getInputs();
    std::vector<std::shared_ptr<AudioOutput>> getOutputs();

    void init();
    void shutdown();
    void update();

    std::shared_ptr<Sound> createSound(std::filesystem::path path);
    std::shared_ptr<AudioListener> createListener();
    std::shared_ptr<AudioSource> createSource();

private:
    Engine& m_engine;

    std::shared_ptr<AudioSystem> m_system;

};

} // namespace bl