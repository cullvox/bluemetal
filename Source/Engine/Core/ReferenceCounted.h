#pragma once

#include <memory>

namespace bl {

template <class T>
using Ref = std::weak_ptr<T>; /** @brief Rename the reference counter to something a little more useful. */

} // namespace bl