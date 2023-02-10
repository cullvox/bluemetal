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
    window{ \
        width=1920 \
        height=1080 \
    } \
    renderer{ \
        antialias=1 \
    } \
";

CConfig::CConfig(const std::string& configPath)
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
    std::ifstream configFile;
    try {
        configFile.open(m_path, std::ios::in | std::ios::binary);
    } catch (const std::exception& except) {
        fmt::print("{}", except.what());
        return;
    }  

    /* Read in the whole config. */
    std::string config;
    std::stringstream buffer;
    buffer << configFile.rdbuf();
    config = buffer.str();

    RemoveLineWhitespaceKeepStringWhitespace(config);

    size_t offset = 0;
    while (offset < config.size())
    {
        auto equalIt = config.find('=');
        auto bracketBeginIt = config.find("{");
        auto bracketEndIt = config.find("}");
    }

    Debug::Log("Finished reading in config.");
}

void CConfig::RemoveLineWhitespaceKeepStringWhitespace(std::string& line)
{
    bool inString = false;
    int pos;
    std::for_each(line.begin(), line.end(), [&](char c){
        inString = (c=='\"' && !inString);
        if (inString) return;
        if (std::isspace(c)) line.erase(pos);
        pos++;
    });

    if (inString) throw std::runtime_error("Unterminated string");
}

SConfigValue CConfig::ParseLine(const std::string& dirtyLine, SParseContext&context)
{

    return SConfigValue();
}