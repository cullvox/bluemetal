#pragma once

namespace bl {

/// @brief Basic singleton class, inherit to gain access to the singleton.
template<class T>
class Singleton {
public:

    /// @brief Returns the singleton object.
    static T* Get() const {
        return _singleton.get();
    }

private:
    static std::unique_ptr<T> _singleton;
};

} // namespace bl