#pragma once

#include "Audio/AudioSource.h"
#include "Audio/AudioListener.h"

namespace bl
{

class Engine;
class AudioOutput;
class AudioInput;

struct AudioSubsystemInitInfo
{
    uint32_t inputId;
    uint32_t outputId;
};

class AudioSubsystem
{
public:

    /// Engine Constructor
    AudioSubsystem(Engine& engine);

    /// Default Destructor
    ~AudioSubsystem();

    void update();

    std::shared_ptr<AudioSystem> getSystem();

    std::string getDriverName();
    std::string getOutputDeviceName();
    std::vector<AudioInput*> getInputs();
    std::vector<AudioOutput> getOutputs();
    std::unique_ptr<Sound> createSound(std::filesystem::path path);
    std::unique_ptr<AudioListener> createListener();
    std::unique_ptr<AudioSource> createSource();

private:
    Engine& _engine;
    std::shared_ptr<AudioSystem> _system;
};

} // namespace bl