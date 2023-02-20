#pragma once

//===========================//
#include <vector>
//===========================//

struct SVideoMode
{
    int width;
    int height;
    int bpp;
    int rr;
};

class IMonitor
{
public:
    IMonitor() = default;
    ~IMonitor() = default;
    virtual const std::vector<SVideoMode>& GetVideoModes() = 0;
};