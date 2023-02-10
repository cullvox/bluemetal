#include <assert.h>

#include <exception>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cctype>

#include <fmt/core.h>
#include <string_view>

#include "core/Debug.hpp"
#include "Config.hpp"


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

const std::string CConfig::DEFAULT_CONFIG = "\
    window{\n \
        width=1920\n \
        height=1080\n \
    }\n \
    renderer{\n \
        antialias=1\n \
    }\n \
";

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
    std::ofstream config("config/config.conf", std::ios::out);
    config << DEFAULT_CONFIG;
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
    if (!std::filesystem::exists("config/config.conf"))
    {
        Reset();
    }
}

void CConfig::ParseInto()
{
    Debug::Log("Opening config file.");

    /* Open the config file. */
    std::ifstream configFile(m_path, std::ios::in);
    if (configFile.bad()) throw std::runtime_error("Could not open config file.");

    /* Read in the whole config. */
    std::string content((std::istreambuf_iterator<char>(configFile)),
                        (std::istreambuf_iterator<char>()));

    RemoveComments(content);
    RemoveWhitespaceKeepStringWhitespace(content);
    Parse(content);

    size_t offset = 0;
    while (offset < content.size())
    {
        auto equalIt = content.find('=');
        auto bracketBeginIt = content.find("{");
        auto bracketEndIt = content.find("}");
    }

    Debug::Log("Finished reading in config.");
}

void CConfig::RemoveComments(std::string& content)
{
    bool inComment = false;
    for (auto it = content.begin(); it != content.end(); )
    {
        if (*it == '#')
            inComment = true;
        else if (*it == '\n')
            inComment = false;
        
        if (inComment)
            it = content.erase(it);
        else
            ++it;
    }
}

void CConfig::RemoveWhitespaceKeepStringWhitespace(std::string& content)
{
    bool inString = false;
    auto it = std::remove_if(content.begin(), content.end(), [&](unsigned char c){
        if (c == '\"' && !inString)
            inString = true;
        else if (c =='\"' && inString)
            inString = false;

        if (inString) return false;
        if (std::isspace(c)) return true;
        return false;
    });

    content.erase(it, content.end());

    if (inString) throw std::runtime_error("Unterminated string");
}

std::string_view CConfig::ParseName(std::string_view& view)
{
    auto equals = view.find('=');
    std::string_view name = view.substr(0, equals);

    view = std::string_view(view.begin() + equals + 1, view.end());
}

void CConfig::ParseEqualType(std::string_view& view)
{

}

void CConfig::Parse(const std::string& content)
{
    std::string_view view = content;
    std::string_view name;
    EEqualType       postEqualType;
    std::string_view value;

    uint32_t index = 0;
    while (view.size() > 0)
    {
        name = ParseName(view);
        postEqualType = ParsePostEquals(view);
        
        switch (postEqualType) {
            case eValue: ParseValue(view);
            case eGroup: ParseGroup(view);
        }


        if (view[0] == '{')
            ParseGroup(view);

    }

}