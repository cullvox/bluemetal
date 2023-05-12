#include "Noodle/Noodle.hpp"
#include "Noodle/NoodleExceptions.hpp"
#include "NoodleLexer.hpp"

blNoodle blNoodle::parse(const std::string& noodle)
{
    blNoodleLexer lexer(noodle);
    blNoodleToken token;

    auto next = [&lexer, &token]()
    {
        token = lexer.next();
    };

    // Get the first token from the lexer
    next();

    std::string identifier;
    std::string value;

    blNoodle root("");
    blNoodle* current = &root;

    // Parse through the file until the file has been completely lexed and parsed
    while (token.kind() != blNoodleTokenKind::eEnd)
    {

        // The first token must be an identifier.
        if (!token.is(blNoodleTokenKind::eIdentifier)) 
        {
            throw blNoodleParseException("expected identifier token");
        }

        identifier = std::string(token.lexeme());

        // After an identifier must come an equals
        next();
        if (!token.is(blNoodleTokenKind::eEqual))
        {
            throw blNoodleParseException("expected equals \'=\' token");
        }
        
        // Must be a left curly, number or a string
        next();
        switch (token.kind())
        {
            case blNoodleTokenKind::eLeftCurly:
            {
                // Insert a new group noodle
                // Consecutive noodles will be inside of this group
                current = &(current->operator[](identifier) = blNoodle(identifier, current));
                next();
                continue;
            }

            case blNoodleTokenKind::eInteger:
            {
                int value = (int)std::strtol(token.lexeme().data(), nullptr, 10);
                current->operator[](identifier) = blNoodle(identifier, value, current);

                next();
                break;
            }

            case blNoodleTokenKind::eFloat:
            {
                float value = (float)std::strtof(token.lexeme().data(), nullptr);
                current->operator[](identifier) = blNoodle(identifier, value, current);

                next();
                break;
            }

            case blNoodleTokenKind::eBoolean:
            {
                bool value = (std::string(token.lexeme()) == "true") ? "true" : "false"; 
                current->operator[](identifier) = blNoodle(identifier, value, current);

                next();
                break;
            }

            case blNoodleTokenKind::eString:
            {
                current->operator[](identifier) = blNoodle(identifier, std::string(token.lexeme()), current);

                next();
                break;
            }

            case blNoodleTokenKind::eLeftBracket:
            {
                blNoodle* array = &(current->operator[](identifier) = blNoodle(identifier, current));
                array->_type = blNoodleType::eArray;
                
                // Get the first element, it determines the type of the array
                next();

                // If it's a right bracket the array has no values right now
                if (token.is(blNoodleTokenKind::eRightBracket))
                {
                    break;
                }

                if (!token.isOneOf(blNoodleTokenKind::eInteger, blNoodleTokenKind::eFloat, 
                                blNoodleTokenKind::eBoolean, blNoodleTokenKind::eString))
                {
                    throw blNoodleParseException("expected array type, integer, float, boolean, string or right bracket");
                }

                blNoodleTokenKind expected = token.kind();

                while (true)
                {
                    // Process the value
                    if (token.kind() != expected)
                    {
                        throw blNoodleParseException("unexpected type change in array");
                    }

                    switch (expected)
                    {
                        case blNoodleTokenKind::eInteger:
                        {
                            int value = (int)std::strtol(token.lexeme().data(), nullptr, 10);
                            array->_array.emplace_back("", value, array);
                            break;
                        }
                        case blNoodleTokenKind::eFloat:
                        {
                            float value = std::strtof(token.lexeme().data(), nullptr);
                            array->_array.emplace_back("", value, array);
                            break;
                        }
                        case blNoodleTokenKind::eBoolean:
                        {
                            bool value = (std::string(token.lexeme()) == "true") ? "true" : "false"; 
                            array->_array.emplace_back("", value, current);
                            break;
                        }
                        case blNoodleTokenKind::eString:
                        {
                            array->_array.emplace_back("", std::string(token.lexeme()), current); 
                            break;
                        }
                    }

                    // Comma is expected or ]
                    next();
                    if (token.kind() == blNoodleTokenKind::eComma)
                    {
                        next();
                        continue;
                    }
                    else if (token.kind() == blNoodleTokenKind::eRightBracket)
                    {
                        next();
                        break;
                    }
                    else
                    {
                        throw blNoodleParseException("expected comma or right bracket in array");
                    }
                }
                break;
            }

            default:
                throw blNoodleParseException("expected left curly \'{\', integer, float or string token");
        }

        // Check if the next value is a comma, if it isn't we expect a right curly
        if (token.kind() == blNoodleTokenKind::eComma)
        {
            next();

            if (token.kind() == blNoodleTokenKind::eRightCurly)
            {
                goto parseCurlyEnds;
            }

            continue;
        }

        parseCurlyEnds:
        
        if (token.kind() == blNoodleTokenKind::eRightCurly)
        {
            // Remove consecutive group endings after the last value
            do 
            {
                if (current->parent())
                {
                    current = current->parent();
                    next();
                }
                else
                {
                    throw blNoodleParseException("unexpected right curly \'}\' token");
                }
                
            } while(token.kind() == blNoodleTokenKind::eRightCurly);

            continue;
        }
        else
        {
            throw blNoodleParseException("expected right curly \'}\' token");
        }
    }

    return root;
}

blNoodle blNoodle::parseFromFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::in);
    
    std::stringstream ss;
    ss << file.rdbuf();

    return parse(ss.str());
}

blNoodle::blNoodle(const std::string& groupName, blNoodle* parent)
    : _name(groupName)
    , _value()
    , _type(blNoodleType::eGroup)
    , _parent(parent)
{
}

blNoodle::blNoodle(const std::string& name, int value, blNoodle* parent)
    : _name(name)
    , _value(value)
    , _type(blNoodleType::eInteger)
    , _parent(parent)
{
}

blNoodle::blNoodle(const std::string& name, float value, blNoodle* parent)
    : _name(name)
    , _value(value)
    , _type(blNoodleType::eFloat)
    , _parent(parent)
{
}

blNoodle::blNoodle(const std::string& name, bool value, blNoodle* parent)
    : _name(name)
    , _value(value)
    , _type(blNoodleType::eBoolean)
    , _parent(parent)
{
}

blNoodle::blNoodle(const std::string& name, const std::string& value, blNoodle* parent)
    : _name(name)
    , _value(value)
    , _type(blNoodleType::eString)
    , _parent(parent)
{
}

blNoodleType blNoodle::arrayType() const
{
    if (_type != blNoodleType::eArray)
    {
        throw blNoodleInvalidAccessException("type was not an array");
    }

    return _arrayType;
}

size_t blNoodle::size() const
{
    switch(_type)
    {
        case blNoodleType::eGroup:
            return _groupChildren.size();
        case blNoodleType::eArray:
            return _array.size();
        default:
            return 0;
    }
}

std::string blNoodle::dump() const noexcept
{
    std::stringstream ss;
    recursiveDump(ss, 0);
    return ss.str();
}

void blNoodle::dumpToFile(const std::filesystem::path& path) const noexcept
{
    std::ofstream file(path);

    std::stringstream ss;
    recursiveDump(ss, 0);
    file << ss.str();
}

std::string blNoodle::toString() const noexcept
{
    switch(_type)
    {
        case blNoodleType::eGroup:
            return _name;
        case blNoodleType::eInteger:
            return std::to_string(std::get<int>(_value));
        case blNoodleType::eFloat:
            return std::to_string(std::get<float>(_value));
        case blNoodleType::eBoolean:
            return std::string(std::get<bool>(_value) ? "true" : "false");
        case blNoodleType::eString:
            return "\"" + std::get<std::string>(_value) + "\"";
        case blNoodleType::eArray:
        {
            std::stringstream ss;
            ss << "[ ";
            bool isFirst = true;
            for (const blNoodle& noodle : _array)
            {
                if (!isFirst)
                    ss << ", ";
                else
                    isFirst = false;

                ss << noodle;
            }
            ss << " ]";
            return ss.str();
        }
        default:
            return std::string();
    }
}

blNoodle& blNoodle::operator=(int value) noexcept
{
    _type = blNoodleType::eInteger;
    _value = value;
    return *this;
}

blNoodle& blNoodle::operator=(float value) noexcept
{
    _type = blNoodleType::eFloat;
    _value = value;
    return *this;
}

blNoodle& blNoodle::operator=(bool value) noexcept
{
    _type = blNoodleType::eBoolean;
    _value = value;
    return *this;
}

blNoodle& blNoodle::operator=(const std::string& value) noexcept
{
    _type = blNoodleType::eString;
    _value = value;
    return *this;
}

blNoodle& blNoodle::operator[](const std::string& key)
{
    if (_type != blNoodleType::eGroup)
    {
        throw blNoodleInvalidAccessException("cannot access a group from a non group noodle");
    }

    auto& noodle = _groupChildren[key];
    noodle._parent = this;

    return noodle;
}

blNoodle& blNoodle::operator[](size_t index)
{
    if (_type != blNoodleType::eArray)
    {
        throw blNoodleInvalidAccessException("cannot access an array from a non array noodle");
    }

    return _array[index];
}

std::ostream& operator<<(std::ostream& os, const blNoodle& noodle)
{
    os << noodle.toString();
    return os;
}

void blNoodle::recursiveDump(std::stringstream& ss, int tabs) const noexcept
{
    auto offset = [&]()
    {
        for (int i = 0; i < tabs; i++)
        {
            ss << '\t';
        }
    };

    offset();

    if (_type == blNoodleType::eGroup)
    {
        // Don't output a name or } on the root node
        if (!isRoot())
        {
            tabs++;
            ss << _name << " = {\n";
        }

        for (const auto& pair : _groupChildren)
        {
            const blNoodle& noodle = pair.second;
            
            noodle.recursiveDump(ss, tabs);
        }

        if (!isRoot())
            tabs--;
        
        offset();

        if (!isRoot())
            ss << "}\n";
        return;
    }

    // Add name and equals sign
    ss << _name << " = " << toString() << ",\n";
}