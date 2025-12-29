#include <chrono>

#include "Core/Print.h"
#include "Profiler.h"

namespace bl {

void Profiler::EnableProfiling(bool enable)
{
    _enabled = enable;
}

bool Profiler::IsProfilingEnabled() const
{
    return _enabled;
}

void Profiler::StartFrame()
{
}

void Profiler::EndFrame()
{
    if (!_enabled) {
        return;
    }

    // Clear previous frame times
    _lastFrameTimes.clear();
    _lastFrameNames.clear();

    // Store the times for the last frame
    for (const auto& [name, time] : _profiles) {
        _lastFrameTimes.push_back(time);
        _lastFrameNames.push_back(name.data());
    }

    _profiles.clear();
}

void Profiler::StartProfile(const std::string_view& name)
{
    if (!_enabled) {
        return;
    }

    // Start timing the profile with the given name.
    _profiles[name] = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
}

void Profiler::EndProfile(const std::string_view& name)
{
    if (!_enabled) {
        return;
    }

    // End timing the profile and calculate the elapsed time.
    auto endTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    if (_profiles.find(name) != _profiles.end()) {
        double startTime = _profiles[name];
        double elapsedTime = endTime - startTime;
        _profiles[name] = elapsedTime; // Store elapsed time

        // Print the profiling result.
        //Print::Info("Profile [{}]: {} ns", name, static_cast<uint64_t>(elapsedTime));
    }
    else {
        //Print::Warn("Profile [{}] was not started!", name);
    }
}

const std::unordered_map<std::string_view, double>& Profiler::GetProfiles() const
{
    return _profiles;
}

void Profiler::GetProfileNames(std::vector<const char*>& names) const
{
    names.clear();
    for (const auto& name : _lastFrameNames) {
        names.push_back(name);
    }
}

void Profiler::GetProfileTimes(std::vector<float>& times) const
{
    times.clear();
    for (auto time : _lastFrameTimes) {
        times.push_back(static_cast<float>(time) / 1'000'000.0f); // Convert to milliseconds
    }
}

Profiler& GetGlobalProfiler()
{
    static Profiler globalProfiler;
    return globalProfiler;
}

} // namespace bl