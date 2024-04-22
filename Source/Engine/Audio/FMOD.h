#pragma once

#include <fmod.hpp>

#define FMOD_CHECK(x) \
    if ((x) != FMOD_OK) { throw std::runtime_error("FMOD Audio Error: " BL_STRINGIFY(x)); }
