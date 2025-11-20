#pragma once

#include "Precompiled.h"

namespace bl {

template <class T>
using Ref = std::shared_ptr<T>; /** @brief Rename the reference counter to something a little more useful. */

} // namespace bl