#pragma once

#include <string_view>
#include <unordered_map>

namespace bl {

class Profiler {
    bool _enabled = true;
    std::unordered_map<std::string_view, double> _profiles;
    std::vector<const char*> _lastFrameNames;
    std::vector<double> _lastFrameTimes;
public:
    Profiler() = default;
    ~Profiler() = default;

    void EnableProfiling(bool enable);
    bool IsProfilingEnabled() const;
    void StartFrame();
    void EndFrame();
    void StartProfile(const std::string_view& name);
    void EndProfile(const std::string_view& name);

    void GetProfileNames(std::vector<const char*>& names) const;
    void GetProfileTimes(std::vector<float>& times) const;

    const std::unordered_map<std::string_view, double>& GetProfiles() const;
};

Profiler& GetGlobalProfiler();

#define BL_PROFILE_START(name) bl::GetGlobalProfiler().StartProfile(name);
#define BL_PROFILE_END(name) bl::GetGlobalProfiler().EndProfile(name);

} // namespace bl