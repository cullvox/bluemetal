#pragma once

#include "Precompiled.h"
#include "Core/MacroUtils.h"
#include "Resources/ResourceManager.h"

#include "FMOD.h"

namespace bl
{

class Engine;
class Sound;

class AudioSystem
{
public:

    /**
     * @brief Resource builder for the resource manager.
     */
    virtual std::unique_ptr<Resource> BuildResource(ResourceManager* manager, const std::string& type);

    /**
     * @brief Returns the singleton instance of the audio system.
     * @return The singleton instance of the engines audio system.
     */
    static AudioSystem* GetInstance();

    /**
     * @brief Returns the FMOD System object.
     * @return The FMOD System object.
     */
    FMOD::System* GetFMOD();

    /**
     * @brief Ticks the audio system along for another frame.
     */
    void Update(); 

    /**
     * @brief Returns the audio driver name.
     * @return The current audio driver name.
     */
    std::string GetDriverName();

    /**
     * @brief Returns how many audio channels are playing.
     * @return The number of audio channels playing.
     */
    int GetNumChannelsPlaying();

private:
    AudioSystem();
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem(AudioSystem&&) = delete;
    ~AudioSystem();
    
    AudioSystem& operator=(const AudioSystem&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;

private:
    static std::unique_ptr<AudioSystem> _system;
    FMOD::System* _fmod;
};

} // namespace bl
