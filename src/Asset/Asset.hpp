#pragma once

#include <string>

class Asset {
public:
    Asset(const std::string& path);
    ~Asset();

    virtual void unload() = 0;
    virtual void load() = 0;
private:
    std::string path;
};