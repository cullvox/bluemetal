#include "Config/Noodle.hpp"
#include "Config/NoodleExceptions.hpp"
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
        if (not token.is(blNoodleTokenKind::eIdentifier)) 
        {
            throw blNoodleParseException("expected identifier token");
        }

        identifier = std::string(token.lexeme());

        // After an identifier must come an equals
        next();
        if (not token.is(blNoodleTokenKind::eEqual))
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

            case blNoodleTokenKind::eString:
            {
                current->operator[](identifier) = blNoodle(identifier, std::string(token.lexeme()), current);

                next();
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