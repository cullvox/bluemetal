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

    BL_LOG(blLogType::eInfo, "Parsed config: {}\n", _path.string());
}

void blConfig::save()
{

    std::ofstream file(_path, std::ios::out);

    int tabs = 0;

    tree.foreach([&](blParserTree& tree){
        
        if (prevParent != tree.getParent())
        {
            tabs--;

            for (int i = 0; i < tabs; i++)
            {
                file << "\t";
            }

            file << "}" << std::endl;
            prevParent = tree.getParent();
        }

        // Tabinate the next name
        for (int i = 0; i < tabs; i++)
        {
            file << "\t";
        }
        

        file << tree.getName() << " = ";

        // If this value is a branch
        if (not tree.isLeaf())
        {
            file << "{" << std::endl;
            prevParent = &tree;
            tabs++;
        }
        else
        {

            // Determine which type of value we need to output
            switch (tree.getValue().index())
            {
                case 0: // integer
                    file << std::to_string(std::get<int>(tree.getValue())) << "," << std::endl; 
                    break;
                case 1: // string
                    file << "\"" << std::get<std::string_view>(tree.getValue()) << "\"," << std::endl;  
                    break;
            }
        }


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