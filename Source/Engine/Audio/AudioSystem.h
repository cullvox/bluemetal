#pragma once

#include "Precompiled.h"
#include "FMOD.h"
#include "Core/MacroUtils.h"
#include "Core/NonCopyable.h"
#include "Resource/ResourceManager.h"
#include "Sound.h"
#include "Listener.h"
#include "Source.h"

namespace bl
{

class Engine;
class Sound;

class AudioSystem : public NonCopyable, public ResourceBuilder
{
public:
    AudioSystem(Engine* engine);
    ~AudioSystem();


    FMOD::System* Get();
    void Update(); /** @brief Ticks the audio system along for another frame. */
    std::string GetDriverName();
    int GetNumChannelsPlaying();

    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& data);

    std::unique_ptr<Sound> CreateSound(std::filesystem::path path);
    std::unique_ptr<Listener> CreateListener();
    std::unique_ptr<Source> CreateSource();

private:
    Engine* _engine;
    FMOD::System* _system;
};

} // namespace bl
