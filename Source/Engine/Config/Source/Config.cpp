#include "Core/Log.hpp"
#include "Config/Config.hpp"
#include "Config/Configurable.hpp"
#include "Config/ConfigParser.hpp"


namespace bl
{

Config::Config(const std::string& configPath)
    : m_path(configPath)
{
    EnsureDirectoriesExist();
    ResetIfConfigDoesNotExist();
    ParseInto();
}

Config::~Config()
{
}

void Config::Reset()
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

bool Config::HasValue(const std::string& name) const noexcept
{
    return m_values.contains(name);
}

int Config::GetIntValue(const std::string& name) const
{
    assert(HasValue(name));
    assert(m_values.at(name).type == EParsedType::eInt);
    return m_values.at(name).i;
}

const std::string_view Config::GetStringValue(const std::string& name) const
{
    assert(HasValue(name));
    assert(m_values.at(name).type == EParsedType::eString);
    
    return m_values.at(name).s;
}

int Config::GetIntOrSetDefault(const std::string& key, int defaultValue)
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

void Config::EnsureDirectoriesExist()
{
    /* Ensure directories exist. */
    if (!std::filesystem::exists("config"))
    {
        std::filesystem::create_directory("config");
        Reset();
    }
}

void Config::ResetIfConfigDoesNotExist()
{
    if (!std::filesystem::exists("Config/Config.nwdl"))
    {
        Reset();
    }
}

void Config::ParseInto()
{
    BL_LOG(blLogType::eInfo, "Opening config: {}\n", m_path);

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
        BL_LOG(blLogType::eDebug, "Value [{}, {}, {}] \n", pair.first,
            (int)(pair.second.type), 
            (pair.second.type == EParsedType::eInt) ? 
                std::to_string(pair.second.i) : 
                std::string{pair.second.s});
    }

    BL_LOG(blLogType::eInfo, "Parsed config: {}\n", m_path);
}

void DetermineGroups(const std::string& key, std::vector<std::string>& groups)
{
    size_t dot = 0, previous = 0;

    groups.clear();

    do
    {
        dot = key.find('.', previous);        
        const size_t count = dot - previous;
        groups.push_back(key.substr(previous, count));
    } while (dot != key.size()-1);
}

void Config::Save()
{
    
    /* Sort the keys so the values will properly be placed in groups. */
    std::vector<std::string> keys;

    keys.reserve (m_values.size());
    for (auto& it : m_values) {
        keys.push_back(it.first);
    }
    std::sort(keys.begin(), keys.end());

    
    std::vector<std::string> previousGroups{};
    for (auto key : keys)
    {

        /* If the groups are the same, we don't need to apply a different grouping structure. */
        std::vector<std::string> currentGroups{}; 
        DetermineGroups(key, currentGroups);

        if (currentGroups != previousGroups)
        {

        }

        /* Find the last '.' and determine if the groups are the same. */
        BL_LOG(blLogType::eInfo, "{}", key);
    }

}

} // namespace bl