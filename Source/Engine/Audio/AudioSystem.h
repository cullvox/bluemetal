#pragma once

#include "Precompiled.h"
#include "FMOD.h"
#include "Core/MacroUtils.h"
#include "Core/NonCopyable.h"
#include "Resources/ResourceManager.h"

namespace bl
{

class Engine;
class Sound;

class AudioSystem : public NonCopyable, public ResourceBuilder
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
    ~AudioSystem();

private:
    static std::unique_ptr<AudioSystem> _system;
    FMOD::System* _fmod;
};

} // namespace bl
