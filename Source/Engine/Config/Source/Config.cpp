#include "Core/Log.hpp"
#include "Config/Config.hpp"
#include "Config/ConfigParser.hpp"


blConfig::blConfig(
        const std::string& defaultConfig, 
        const std::filesystem::path& configPath)
    : _path(configPath)
{
    ensurePathExists();
    parseInto();
}

blConfig::~blConfig()
{
}

void blConfig::reset()
{
    std::ofstream config(_path, std::ios::out);
    config << _defaultConfig;
}

bool blConfig::hasValue(
    const std::string& name) const noexcept
{
    return _values.contains(name);
}

void blConfig::parseInto()
{
    BL_LOG(blLogType::eInfo, "Opening config: {}", _path.string());

    // Open the config file.
    std::ifstream configFile(_path, std::ios::in);
    if (configFile.bad()) throw std::runtime_error("Could not open config file.");

    // Read in the whole config.
    std::string content((std::istreambuf_iterator<char>(configFile)),
                        (std::istreambuf_iterator<char>()));

    blParser parser{content};
    _values = parser.getValues();

    for (auto pair : _values)
    {
        BL_LOG(blLogType::eDebug, "Value [{}, {}, {}] \n", pair.first,
            pair.second.index(), 
            pair.second.index() == 0 ? 
                std::to_string(std::get<int>(pair.second)) : 
                std::string{std::get<std::string_view>(pair.second)});
    }

    BL_LOG(blLogType::eInfo, "Parsed config: {}\n", _path.string());
}

void blConfig::save()
{

}

void blConfig::ensurePathExists()
{
    if (not std::filesystem::exists(_path.parent_path()))
    {
        if (not std::filesystem::create_directory(_path.parent_path()))
        {
            throw std::runtime_error("Could not create a config path!");
        }

        reset();
    }
}