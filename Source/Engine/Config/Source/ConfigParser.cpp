#include "Core/Log.hpp"
#include "Config/ConfigParser.hpp"


CParser::CParser(const std::string& content) noexcept
    : m_lexer(content), m_token()
{
}

const std::unordered_map<std::string, SParsedValue>& CParser::Parse()
{
    Next();
    while (m_tokenKind != ETokenKind::eEnd)
    {
        std::string_view identifier{};
        std::string_view value{};

        /* The first token must be an identifier. */
        if (not m_token.Is(ETokenKind::eIdentifier)) 
        {
            PrintError("Identifier");
            return m_values;
        }
        identifier = m_token.Lexeme();

        /* Must be an equals. */
        Next();
        if (not m_token.Is(ETokenKind::eEqual))
        {
            PrintError("=");
            return m_values;
        }
        
        /* Must be a left-bracket or a number or a string */
        Next();
        switch (m_tokenKind)
        {
            case ETokenKind::eLeftCurly:
            {
                m_groups.push_back(identifier);
                RecomputeGroups();
                Next();
                continue;
            }
            case ETokenKind::eNumber:
            {
                m_values.insert({
                    m_groupsStr + std::string{identifier}, 
                    SParsedValue{ 
                        .type = EParsedType::eInt, 
                        .i = strtol(m_token.Lexeme().data(), nullptr, 10)
                    }
                });
                Next();
                break;
            }
            case ETokenKind::eString:
            {
                m_values.insert({
                    m_groupsStr + std::string{identifier}, 
                    SParsedValue{ 
                        .type = EParsedType::eString, 
                        .s = m_token.Lexeme()
                    }
                });
                Next();
                break;
            }
            default:
            {
                PrintError("}, Number, or String");
                return m_values;
            }
        }

        /* Check if the group ended after this value. */
        if (m_tokenKind == ETokenKind::eRightCurly)
        {
            do {
                if (m_groups.size() > 0)
                {
                    m_groups.pop_back();
                    RecomputeGroups();
                    Next();
                }
                else
                {
                    PrintError("}");
                    return m_values;
                }
                
            }
            while (m_tokenKind == ETokenKind::eRightCurly);
            continue;
        }

        /* A comma is expected if the group did not end. */
        else if (m_tokenKind != ETokenKind::eComma)
        {
            PrintError(",");
            return m_values;
        }
        
        Next();
    }

    return m_values;
}

void CParser::Next()
{
    m_token = m_lexer.Next();
    m_tokenKind = m_token.Kind();
}

void CParser::RecomputeGroups()
{
    m_groupsStr.clear();
    for (const std::string_view& view : m_groups)
        m_groupsStr += std::string{view} + ".";
}

void CParser::PrintError(const std::string& expected)
{
    Logger::Error("({}): Invalid token, expected \"{}\", got \"{}\".\n", m_lexer.Line(), expected, m_token.Lexeme());
}