#include "Core/Log.hpp"
#include "Config/ConfigParser.hpp"


blParser::blParser(const std::string& content) noexcept
    : _lexer(content)
    , _token()
    , _tree()
{
    parse();
}

const blParserTree& blParser::getTree()
{
    return _tree;
}

void blParser::parse()
{
    // Get the first token from the lexer
    next();

    blParserTree* current = &_tree;
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
                current = &(current->addBranch(std::string(identifier)));
                
                next();
                continue;
            }

            case blTokenKind::eNumber:
            {
                current->addLeaf(std::string(identifier), strtol(_token.getLexeme().data(), nullptr, 10));

                next();
                break;
            }

            case blTokenKind::eString:
            {
                current->addLeaf(std::string(identifier), std::string(_token.getLexeme()));

                next();
                break;
            }

            default:
            {
                printError("{, Number, or String");
                return;
            }
        }

        // Check if the next value is a comma, if it isn't we expect a right curly
        if (_tokenKind == blTokenKind::eComma)
        {
            next();

            if (_tokenKind == blTokenKind::eRightCurly)
            {
                goto parseCurlyEnds;
            }

            continue;
        }

        parseCurlyEnds:
        
        if (_tokenKind == blTokenKind::eRightCurly)
        {
            // Remove consecutive group endings after the last value
            do 
            {
                if (current->getParent())
                {
                    current = current->getParent();
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
        else
        {
            printError("}");
        }
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