#include <cctype>
#include <cassert>
#include <cstdlib>
#include <string>
#include <fstream>
#include <algorithm>
#include <filesystem>

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
    # Default configuration\n \
    window={\n \
        width=1920;\n \
        height=1080;\n \
    }\n \
    renderer={\n \
        antialias=1;\n \
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
    std::ofstream config(DEFAULT_CONFIG_PATH, std::ios::out);
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
    if (!std::filesystem::exists(DEFAULT_CONFIG_PATH))
    {
        Reset();
    }
}


struct Value
{
    std::string_view name;
    std::string_view value;
};
struct Group
{
    std::string_view name;
    std::vector<Value> values;
};

enum class ParsingState
{
    None,
    ReadName,
    ReadValue,
};

void Tokenize(const std::string& content)
{

    Group defaultGroup{};

    size_t size;
    ParsingState state = ParsingState::ReadName;
    std::string_view view = content;
    std::string_view name;
    std::string_view value;

    while (view.size() > 0)
    {

        auto groupTokenizer = [&]() {
            
        };

        auto valueTokenizer = [&]() {
            
            auto it = view.find('=');
            auto name = view.substr(0, it);
            view = std::string_view(view.begin()+it+1, view.end());

            /* Special check for a group. */
            switch (view[0])
            {
                case '{': 
                    groupTokenizer();
                    break;
                case '\"':
                    value = std::string_view(view.begin()+1, view.find('\''));
            }            
        };

        valueTokenizer();
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
    
   Tokenize(content);
    //ParseValue(view);

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
    return name;
}

std::string_view CConfig::TokenizeGroup(std::string_view& content)
{
    uint32_t pos = 0;
    uint32_t groupsCount = 0;
    char c = content[0];

    do 
    {
        c = content[pos];
        if (c == '{') groupsCount++;
        if (c == '}') groupsCount--;
        pos++;
    } while (groupsCount != 0);

    return std::string_view(content.begin(), content.begin() + pos);
}


std::string_view CConfig::TokenizeValue(std::string_view& content)
{
    return {};
}

void CConfig::ParseGroup(std::string_view& view)
{
    while (true)
    {
        /* Check if the group is over. */
        if (view[0] == '}')
        {
            return;
        }
        
        /* Get the name of the value. */
        std::string_view name = ParseName(view);
        ParseValue(name, view);
    }
}

void CConfig::ParseValue(const std::string_view& name, std::string_view& view)
{
    
    /* Check if this value is starting another group. */
    if (view[0] == '{')
    {   
        std::string_view group = TokenizeGroup(view);
        ParseGroup(group);
        return;
    }

    /* Get the substring for the actual value. */
    std::string_view value = view.substr(0, view.find(';'));
    view = std::string_view(value.begin(), view.end());

    /* Determine the string type. */
    char* strend = nullptr;

    long i = std::strtol(value.data(), &strend, 10);
    if (strend == value.end())
    {
        m_values.push_back((SConfigValue){std::string(name), EConfigType::eInteger, true, i, 0.0f, ""});
        return;
    }

    float f = std::strtof(value.data(), &strend);
    if (strend == value.end())
    {
        m_values.push_back((SConfigValue){std::string(name), EConfigType::eFloat, true, 0, 0.f, ""});
        return;
    }

    if (value == "true")
    {
        m_values.push_back((SConfigValue){std::string(name), EConfigType::eBoolean, true, 0, 0.0f, ""});
        return;
    } else if (value == "false") 
    {
        m_values.push_back((SConfigValue){std::string(name), EConfigType::eBoolean, false, 0, 0.0f, ""});
        return;
    } else if (value[0] == '\"')
    {
        //m_values.push_back((SConfigValue){std::string(name)})
    }
    

}