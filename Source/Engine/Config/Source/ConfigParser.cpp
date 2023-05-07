#include "Core/Log.hpp"
#include "Config/ConfigParser.hpp"


blParser::blParser(const std::string& content) noexcept
    : _lexer(content)
    , _token()
    , _tree()
{
    parse();
}

std::unordered_map<std::string, blParsedValue> blParser::getValues()
{
    std::unordered_map<std::string, blParsedValue> values;
    _tree.recurseBuildValues(values);
    return values;
}

void blParser::parse()
{
    // Get the first token from the lexer
    next();

    blParserTree& current = _tree;
    std::string_view identifier{};
    std::string_view value{};

    // Parse through the file until the file has been completely lexed and parsed
    while (_tokenKind != blTokenKind::eEnd)
    {

        // The first token must be an identifier.
        if (not _token.is(blTokenKind::eIdentifier)) 
        {
            printError("value name");
            return;
        }

        identifier = _token.getLexeme();

        next();

        // After an identifier must come an equals
        if (not _token.is(blTokenKind::eEqual))
        {
            printError("=");
            return;
        }
        
        next();


        // Must be a left curly, number or a string
        switch (_tokenKind)
        {
            case blTokenKind::eLeftCurly:
            {

                // Consecutive identifiers will be inside of this group
                current = current.addChild(identifier);
                
                next();
                continue;
            }

            case blTokenKind::eNumber:
            {
                const blParsedValue value = strtol(_token.getLexeme().data(), nullptr, 10);
                current.addChild(identifier, value);

                next();
                break;
            }

            case blTokenKind::eString:
            {
                const blParsedValue value = _token.getLexeme();
                current.addChild(identifier, value);

                next();
                break;
            }

            default:
            {
                printError("{, Number, or String");
                return;
            }
        }

        // Check if the group ended after this value.
        if (_tokenKind == blTokenKind::eRightCurly)
        {

            // Remove consecutive group endings after the last value
            do 
            {
                if (current.getParent())
                {
                    current = *current.getParent();
                    next();
                }
                else
                {
                    printError("identifier");
                    return;
                }
                
            } while (_tokenKind == blTokenKind::eRightCurly);

            continue;
        }

        // A comma is expected if the group did not end.
        else if (_tokenKind != blTokenKind::eComma)
        {
            printError(",");
            return;
        }
        
        next();
    }
}

void blParser::next()
{
    _token = _lexer.next();
    _tokenKind = _token.getKind();
}

void blParser::printError(const std::string& expected)
{
    BL_LOG(blLogType::eError, "Config parsing errore: ({},{}): Invalid token, expected \"{}\", got \"{}\".\n", _lexer.getLineNumber(), _lexer.getCharacterNumber(), expected, _token.getLexeme());
}