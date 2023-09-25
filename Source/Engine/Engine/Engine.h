#pragma once


#include "Export.h"
#include "Precompiled.h"
#include "AudioSubsystem.h"
#include "GraphicsSubsystem.h"
#include "Core/Flags.h"
#include "Noodle/Noodle.h"

namespace bl
{

enum class SubsystemFlagBits
{
    eSubsystemResourcesBit      = bl::bit(0),
    eSubsystemGraphicsBit       = bl::bit(1),
    eSubsystemImGuiBit          = bl::bit(2),
    eSubsystemAudioBit          = bl::bit(3),
    eSubsystemWorldBit          = bl::bit(4),
};

using SubsystemFlags = uint32_t;

struct SubsystemInitInfo
{
    const GraphicsSubsystemInitInfo* pGraphicsInit;
};

class BLUEMETAL_API Engine
{
public:

    /// Default Constructor.
    Engine(int argc, const char** argv);

    /// Default Destructor.
    ~Engine();

    /// Initializes parts of the engine that haven't been initialized yet.
    ///
    ///     @param flags Specify subsystems to initialize.
    ///     @param info Customize parts of the engines initialization, so you don't have to later.
    ///
    /// Can call multiple times, if a subsystem has already initialzed it will be skipped.
    ///     
    bool init(SubsystemFlags flags, const SubsystemInitInfo* info = nullptr);

    /// Shuts down all subsystems.
    void shutdown();

    /// Returns a noodle to the config.
    Noodle& getConfig();

    /// Returns the graphics subsystem.
    GraphicsSubsystem* getGraphics();

    /// Returns the audio subsystem.
    AudioSubsystem* getAudio();


    /// Begins the engines game loop.
    void run();
    
private:
    Noodle config;
    std::unique_ptr<GraphicsSubsystem> graphics;
    std::unique_ptr<AudioSubsystem> audio;

};

} // namespace bl
