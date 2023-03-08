#include "Core/Log.hpp"
#include "Config/Config.hpp"
#include "Config/Configurable.hpp"
#include "Config/ConfigParser.hpp"

#include <cctype>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <map>

namespace bl
{

IConfigurable::IConfigurable(CConfig *const pConfig)
    : m_pConfig(pConfig)
{
    assert(m_pConfig);
    m_pConfig->SubscribeEvents(this);
}

IConfigurable::~IConfigurable()
{
    if (m_pConfig) m_pConfig->UnsubscribeEvents(this);
}


CConfig::CConfig(const std::string& configPath)
    : m_path(configPath)
{
    EnsureDirectoriesExist();
    ResetIfConfigDoesNotExist();
    ParseInto();
}

CConfig::~CConfig()
{
}

void CConfig::Reset()
{
    const std::string DEFAULT_CONFIG_PATH = "config/config.nwdl";
    const std::string DEFAULT_CONFIG =
        "# Default configuration\n \
        window = { \n \
            width = 1920; \n \
            height = 1080; \n \
    }\n \
        renderer = { \n \
            antialias = 1; \n \
    }\n \
        ";

    std::ofstream config(DEFAULT_CONFIG_PATH, std::ios::out);
    config << DEFAULT_CONFIG;
}

bool CConfig::HasValue(const std::string& name) const noexcept
{
    return m_values.contains(name);
}

int CConfig::GetIntValue(const std::string& name) const
{
    assert(HasValue(name));
    assert(m_values.at(name).type == EParsedType::eInt);
    return m_values.at(name).i;
}

const std::string_view CConfig::GetStringValue(const std::string& name) const
{
    assert(HasValue(name));
    assert(m_values.at(name).type == EParsedType::eString);
    
    return m_values.at(name).s;
}

int CConfig::GetIntOrSetDefault(const std::string& key, int defaultValue)
{
    if (HasValue(key))
    {
        assert(m_values.at(key).type == EParsedType::eInt);
        
        return m_values[key].i;
    }
    else
    {
        m_values[key].i = defaultValue;
        return defaultValue;
    }
}

void CConfig::SubscribeEvents(const IConfigurable* pConfigurable)
{
    m_subscribers.push_back(pConfigurable);
}

void CConfig::UnsubscribeEvents(const IConfigurable* pConfigurable)
{
    m_subscribers.erase(std::find(m_subscribers.begin(), m_subscribers.end(), pConfigurable));
}

void CConfig::EnsureDirectoriesExist()
{
    /* Ensure directories exist. */
    if (!std::filesystem::exists("config"))
    {
        std::filesystem::create_directory("config");
        Reset();
    }
}

void CConfig::ResetIfConfigDoesNotExist()
{
    if (!std::filesystem::exists("Config/Config.nwdl"))
    {
        Reset();
    }
}

void CConfig::ParseInto()
{
    Logger::Debug("Opening config: {}\n", m_path);

    /* Open the config file. */
    std::ifstream configFile(m_path, std::ios::in);
    if (configFile.bad()) throw std::runtime_error("Could not open config file.");

    /* Read in the whole config. */
    std::string content((std::istreambuf_iterator<char>(configFile)),
                        (std::istreambuf_iterator<char>()));

    CParser parser{content};
    m_values = parser.Parse();

    for (auto pair : m_values)
    {
        Logger::Debug("Value [{}, {}, {}] \n", pair.first, (int)(pair.second.type), (pair.second.type == EParsedType::eInt) ? std::to_string(pair.second.i) : std::string{pair.second.s});
    }

    Logger::Debug("Parsed config: {}\n", m_path);
}

std::vector<std::string> DetermineGroups(const std::string& key)
{
    std::vector<std::string> groups;
    auto startDot = key.begin();

    for (auto it = key.begin(); it < key.end(); it++)
    {
        switch (*it)
        {
            case '.':
                groups.push_back(std::string{startDot, it});
                startDot = it;
                break;
            case '\0':
                break;
            default:
                continue;
        }
    }

    return groups;
}

void CConfig::Save()
{
    
    /* Sort the keys so the values will properly be placed in groups. */
    std::vector<std::string> keys;

    keys.reserve (m_values.size());
    for (auto& it : m_values) {
        keys.push_back(it.first);
    }
    std::sort(keys.begin(), keys.end());

    
    std::vector<std::string> groups;


    for (auto it : keys)
    {

        /* If the groups are the same, we don't need to apply a different grouping structure. */
        std::vector<std::string> currentGroups = DetermineGroups(it);
        if (currentGroups != groups)
        

        /* Find the last '.' and determine if the groups are the same. */
        spdlog::log(spdlog::level::info, "{}", it);
    }

}

} // namespace bl