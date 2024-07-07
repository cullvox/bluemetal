#pragma once

#include "Core/Precompiled.hpp"

namespace bl {

template <typename... TArgs>
class MulticastDelegate;

/** \brief Custom doubly linked list containing std::function's with a specific signature.
 *
 * A list of function callbacks used later and called all together at once or individually.
 * When a new callback is added a handle to the function is returned allowing removal.
 * This requirement/feature was made because std::function objects themselves cannot be compared.
 *
 * \since BloodLust 1.0.0
 *
 */
template <typename... TArgs>
class MulticastDelegate<void(TArgs...)> {
public:
    /** \brief A callback function type. */
    using Func = std::function<void(TArgs...)>;

    MulticastDelegate() = default;
    ~MulticastDelegate() = default;

    /** \brief Runs all the callbacks with the following arguments.
     *
     * All callbacks are executed with the same argument. All return
     * values are discarded because it didn't make sense to only return
     * the last callback value like a delegate would.
     *
     * \param args The templated arguments of the callback themselves.
     *
     * \since BloodLust 1.0.0
     *
     */
    void operator()(TArgs... args) const {
        for (const auto& cb : _callbacks)
            cb(std::forward<TArgs>(args)...);
    }

    /** \brief Adds a new callback to the list.
     *
     * Appends a new callback to the end of the list. Returns a handle iterator
     * to the latest callback added so that it can be removed later.
     *
     *
     */
    void add(const Func& cb) noexcept {
        _callbacks.push_back(cb);
    }

    void operator+=(const Func& cb) {
        add(cb);
    }

    /** \brief Clears all callbacks, invalidates all handles. */
    void clear() noexcept { _callbacks.clear(); }

    size_t size() const noexcept { return _callbacks.size(); }

private:
    std::list<Func> _callbacks;
};

} // namespace bl