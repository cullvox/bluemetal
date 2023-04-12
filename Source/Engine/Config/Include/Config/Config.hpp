#pragma once

#include "Core/Precompiled.hpp"
#include "Config/ConfigParser.hpp"
#include "Config/Export.h"

namespace bl
{

class BLOODLUST_CONFIG_API Config
{

public:
    
    Config(const std::string& path = "Save/Config.nwdl");
    
    ~Config();

    bool HasValue(const std::string&) const noexcept;
    void RequireStringValue(const std::string&, const std::string&);
    void RequireIntValue(const std::string&, int);
    int  GetIntValue(const std::string&) const;
    const std::string_view GetStringValue(const std::string&) const;
    void SetIntValue(const std::string&, int);
    void SetStringValue(const std::string&, const std::string&);

    int GetIntOrSetDefault(const std::string&, int);

    void Reset();
    void Save();

private:
    void EnsureDirectoriesExist();
    void ResetIfConfigDoesNotExist();
    void ParseInto();
    void NotifySubscribers();

    struct SValueNode
    {
        std::vector<SValueNode*> children;
        bool isValue;
        std::string name;
    };

    SValueNode*                                     m_pConfigTree;
    std::string                                     m_path;
    std::unordered_map<std::string, SParsedValue>   m_values;
};

} // namespace bl