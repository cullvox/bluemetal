#include "RandomSound.h"

namespace bl
{

RandomSound::RandomSound(ResourceSystem& resourceSystem, AudioSystem* system, const std::filesystem::path& path)
    : Sound(resourceSystem, system, path)
{

}


} // namespace bl