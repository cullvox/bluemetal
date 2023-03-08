#pragma once

#include "Core/Export.h"
#include "Config/ConfigParser.hpp"

#include <stdexcept>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace bl
{

class IConfigurable;
class BLOODLUST_API CConfig
{
public:
    CConfig(const std::string& configPath = "Config/Config.nwdl");
    ~CConfig();

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

    void SubscribeEvents(IConfigurable const*);
    void UnsubscribeEvents(IConfigurable const*);
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
    std::vector<const IConfigurable*>               m_subscribers;
};

} // namespace bl