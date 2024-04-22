#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "FMOD.h"
#include "Core/MacroUtils.h"
#include "Core/NonCopyable.h"
#include "Resource/ResourceManager.h"

namespace bl
{

class Engine;
class Sound;

class AudioSystem : public NonCopyable, public ResourceBuilder
{
public:
    AudioSystem(Engine& engine);
    ~AudioSystem();


    FMOD::System* Get();
    void Update(); /** @brief Ticks the audio system along for another frame. */
    std::string GetDriverName();
    int GetNumChannelsPlaying();

    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path);

    ResourceRef<Sound> CreateSound(std::filesystem::path path);

private:
    Engine& _engine;
    FMOD::System* _system;
};

} // namespace bl
