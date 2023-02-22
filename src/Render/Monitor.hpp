#pragma once

//===========================//
#include <vector>
//===========================//

struct SVideoMode {
    int width;
    int height;
    int bitsPerPixel;
    int refreshRate;
};

class CMonitor {
public:
    CMonitor() = default;
    ~CMonitor() = default;

    static std::vector<std::unique_ptr<CMonitor>>& GetMonitors();

    virtual const std::vector<SVideoMode>& GetVideoModes() = 0;
};