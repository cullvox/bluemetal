#include "Core/Log.hpp"
#include "Config/ConfigParser.hpp"


blParser::blParser(const std::string& content) noexcept
    : _lexer(content)
    , _token()
{
}

const std::unordered_map<std::string, blParsedValue>& blParser::parse()
{
    next();

    std::string_view identifier{};
    std::string_view value{};

    while (_tokenKind != blTokenKind::eEnd)
    {

        // The first token must be an identifier.
        if (not _token.is(blTokenKind::eIdentifier)) 
        {
            printError("value name");
            return _values;
        }

        identifier = _token.getLexeme();

        /* Must be an equals. */
        next();
        if (not _token.is(blTokenKind::eEqual))
        {
            printError("=");
            return _values;
        }
        
        // Must be a left curly, number or a string
        next();

        switch (_tokenKind)
        {
            case blTokenKind::eLeftCurly:
            {
                // Instead of generating a new value create a new group
                _groups.push_back(identifier);
                recomputeGroups();
                next();
                continue;
            }

            case blTokenKind::eNumber:
            {
                _values.insert({
                    _groupsStr + std::string{identifier}, 
                    blParsedValue{  
                        strtol(_token.getLexeme().data(), nullptr, 10)
                    }
                });
                next();
                break;
            }

            case blTokenKind::eString:
            {
                _values.insert({
                    _groupsStr + std::string{identifier}, 
                    blParsedValue{_token.getLexeme()}
                });
                next();
                break;
            }

            default:
            {
                printError("{, Number, or String");
                return _values;
            }
        }

        // Check if the group ended after this value.
        if (_tokenKind == blTokenKind::eRightCurly)
        {

            // Remove consecutive group endings after the last value
            do 
            {
                if (_groups.size() > 0)
                {
                    _groups.pop_back();
                    recomputeGroups();
                    next();
                }
                else
                {
                    printError("}");
                    return _values;
                }
                
            } while (_tokenKind == blTokenKind::eRightCurly);
            continue;
        }

        /* A comma is expected if the group did not end. */
        else if (_tokenKind != blTokenKind::eComma)
        {
            printError(",");
            return _values;
        }
        
        next();
    }

    return _values;
}

void blParser::next()
{
    _token = _lexer.next();
    _tokenKind = _token.getKind();
}

void blParser::recomputeGroups()
{
    _groupsStr.clear();
    for (const std::string_view& view : _groups)
        _groupsStr += std::string{view} + ".";
}

void blParser::printError(const std::string& expected)
{
    BL_LOG(blLogType::eError, "Config parsing errore: ({},{}): Invalid token, expected \"{}\", got \"{}\".\n", _lexer.getLineNumber(), _lexer.getCharacterNumber(), expected, _token.getLexeme());
}