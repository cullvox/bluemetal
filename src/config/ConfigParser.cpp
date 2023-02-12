#include <queue>

#include "ConfigParser.hpp"

CParser::CParser(const std::string& content) noexcept
    : m_content(content), m_lexer(m_content), m_token()
{
}

const std::unordered_map<std::string, CParsedValue>& CParser::Parse() noexcept
{

    Next();
    while (m_tokenKind != ETokenKind::eEnd)
    {
        std::string_view identifier{};
        std::string_view value{};

        /* The first token must be an identifier. */
        if (m_tokenKind != ETokenKind::eIdentifier) throw;
        identifier = m_token.Lexeme();

        /* Must be an equals. */
        Next();
        if (m_tokenKind != ETokenKind::eEqual) throw;
        
        /* Must be a left-bracket or a number or a string */
        Next();
        switch (m_tokenKind)
        {
            case ETokenKind::eLeftCurly:
                m_groups.push_back(identifier);
                RecomputeGroups();
                Next();
                continue;
            case ETokenKind::eNumber:
                m_values.insert({
                    m_groupsStr + std::string{identifier}, 
                    CParsedValue{ 
                        .type = EParsedType::eInt, 
                        .i = strtol(m_token.Lexeme().data(), nullptr, 10)
                    }
                });
                Next();
                break;
            case ETokenKind::eString:
                m_values.insert({
                    m_groupsStr + std::string{identifier}, 
                    CParsedValue{ 
                        .type = EParsedType::eString, 
                        .s = m_token.Lexeme()
                    }
                });
                Next();
                break;
            default: throw;
        }

        /* Check if the group ended after this value. */
        if (m_tokenKind == ETokenKind::eRightCurly)
        {
            do {
                m_groups.pop_back();
                RecomputeGroups();
                Next();
            }
            while (m_tokenKind == ETokenKind::eRightCurly);
            continue;
        }

        /* A comma is expected if the group did not end. */
        else if (m_tokenKind != ETokenKind::eComma) throw;
        
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