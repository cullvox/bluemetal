#pragma once

#include "Core/MacroUtils.h"
#include "Core/System.h"
#include "Precompiled.h"

#include "FMOD.h"

namespace bl {

class Sound;

class AudioSystem : public System {
    FMOD::System* _fmod;

public:
    AudioSystem(Engine& engine);
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem(AudioSystem&&) = delete;
    ~AudioSystem();
    AudioSystem& operator=(const AudioSystem&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;

    std::shared_ptr<Resource> ConstructResource(ResourceSystem* resourceSystem, std::size_t typeHash, const std::filesystem::path& path) override;

    FMOD::System* GetFMOD(); /** @brief Returns the internal FMOD system. */
    void Update(); /** @brief Ticks the audio system. */
    std::string GetDriverName();
    int GetNumChannelsPlaying();
};

} // namespace bl
