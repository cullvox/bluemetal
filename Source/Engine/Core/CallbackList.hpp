#pragma once

#include "Core/Precompiled.hpp"

namespace bl {

template <typename TReturn, typename... TArgs>
class CallbackList;

/** \brief Custom doubly linked list containing std::function's with a specific signature.
 *
 * A list of function callbacks used later and called all together at once or individually.
 * When a new callback is added a handle to the function is returned allowing removal.
 * This requirement/feature was made because std::function objects themselves cannot be compared.
 *
 * \since BloodLust 1.0.0
 *
 */
template <typename TReturn, typename... TArgs>
class CallbackList<TReturn(TArgs...)> {
    using _Callback = std::function<TReturn(TArgs...)>;
    using _Handle = std::list<_Callback>::iterator;

    std::list<_Callback> _callbacks;

public:
    /** \brief A callback function type. */
    using Callback = _Callback;

    /** \brief A handle is defined as an iterator of the list giving a value to remove. */
    using Handle = _Handle;

    blCallbackList() = default;
    ~blCallbackList() = default;

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
    void operator()(TArgs... args) const
    {
        for (const auto& cb : _callbacks)
            cb(std::forward<TArgs>(args)...);
    }

    /** \brief Run a specific callback.
     *
     * Using the callbacks handle and given arguments, this interpretation
     * of the operator() also returns properly from the callback.
     *
     * \param handle The iterator handle of the callback.
     * \param args Arguments to forward to the callback.
     *
     * \since BloodLust 1.0.0
     *
     */
    TReturn operator()(Handle handle, TArgs... args) const
    {
        return (*handle)(std::forward<TArgs>(args)...);
    }

    /** \brief Adds a new callback to the list.
     *
     * Appends a new callback to the end of the list. Returns a handle iterator
     * to the latest callback added so that it can be removed later.
     *
     *
     */
    Handle add(const Callback& cb) noexcept
    {
        _callbacks.push_back(cb);

        auto it = _callbacks.end();
        it--;

        return it;
    }

    /** \brief Removes a callback from the list, must be a valid handle. */
    void remove(Handle handle) noexcept { _callbacks.erase(handle); }

    /** \brief Clears all callbacks, invalidates all handles. */
    void clear() noexcept { _callbacks.clear(); }

    size_t size() const noexcept { return _callbacks.size(); }
};

} // namespace bl