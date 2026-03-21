#pragma once

#include "Sound.h"

namespace bl
{

class RandomSound : public Sound
{
    using SoundAndProbability = std::pair<Ref<Sound>, float>;
    std::vector<SoundAndProbability> _sounds;

public:
    RandomSound(Engine& engine, const std::filesystem::path& path);
    ~RandomSound();

    void AddSound(int index, Ref<Sound> sound, float probability = 1.0f);
    void RemoveSound(int index);

};


}