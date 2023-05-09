#include "Core/Log.hpp"
#include "Config/Config.hpp"
#include "Config/ConfigParser.hpp"


blConfig::blConfig(
        const std::string& defaultConfig, 
        const std::filesystem::path& configPath)
    : _defaultConfig(defaultConfig)
    , _path(configPath)
    , _tree()
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

bool blConfig::hasValue(const std::string& name) noexcept
{
    return _tree.find(name) != nullptr;
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
    _tree = parser.getTree();

    BL_LOG(blLogType::eInfo, "Parsed config: {}\n", _path.string());
}

void blConfig::save()
{

    std::ofstream file(_path, std::ios::out);

    int depth = 0;

    _tree.foreach([&](blParserTree& node){

        // Don't precess leafs, only process them from branches
        if (node.isLeaf())
            return;        

        // Tabinate the next name
        for (int i = 0; i < depth; i++)
        {
            file << "\t";
        }

        file << node.getName() << " = {" << std::endl;

        depth++;

        for (auto& pair : node.getChildren())
        {
            auto& child = pair.second;

            // Skip branches, let the foreach find them
            if (child.isBranch())
                continue;

            // Tabinate the next name
            for (int i = 0; i < depth; i++)
            {
                file << "\t";
            }

            file << child.getName() << " = ";

            // Determine which type of value we need to output
            switch (child.getValue().index())
            {
                case 0: // integer
                    file << std::to_string(std::get<int>(child.getValue())) << "," << std::endl; 
                    break;
                case 1: // string
                    file << "\"" << std::get<std::string>(child.getValue()) << "\"," << std::endl;  
                    break;
            }
        }

        depth--;

        // Tabinate the next name
        for (int i = 0; i < depth; i++)
        {
            file << "\t";
        }

        file << "}" << std::endl;
    });

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