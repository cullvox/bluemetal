#include <cctype>
#include <cassert>
#include <cstdlib>
#include <string>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "core/Debug.hpp"
#include "Config.hpp"



//==============================================================================
// IConfigurable
//==============================================================================

IConfigurable::IConfigurable(CConfig *const pConfig)
    : m_pConfig(pConfig)
{
    assert(m_pConfig);
    m_pConfig->SubscribeEvents(this);
}

IConfigurable::~IConfigurable()
{
    if (m_pConfig) m_pConfig->UnsubscribeEvents(this);
}


//==============================================================================
// CConfig
//==============================================================================

const std::string CConfig::DEFAULT_CONFIG = "\
    # Default configuration\n \
    window={\n \
        width=1920;\n \
        height=1080;\n \
    }\n \
    renderer={\n \
        antialias=1;\n \
    }\n \
";

CConfig::CConfig(const std::string& configPath)
    : m_path(configPath)
{
    EnsureDirectoriesExist();
    ResetIfConfigDoesNotExist();
    ParseInto();
}

CConfig::~CConfig()
{
}

void CConfig::Reset()
{
    std::ofstream config(DEFAULT_CONFIG_PATH, std::ios::out);
    config << DEFAULT_CONFIG;
}

void CConfig::SubscribeEvents(const IConfigurable* pConfigurable)
{
    m_subscribers.push_back(pConfigurable);
}

void CConfig::UnsubscribeEvents(const IConfigurable* pConfigurable)
{
    m_subscribers.erase(std::find(m_subscribers.begin(), m_subscribers.end(), pConfigurable));
}

void CConfig::EnsureDirectoriesExist()
{
    /* Ensure directories exist. */
    if (!std::filesystem::exists("config"))
    {
        std::filesystem::create_directory("config");
        Reset();
    }
}

void CConfig::ResetIfConfigDoesNotExist()
{
    if (!std::filesystem::exists(DEFAULT_CONFIG_PATH))
    {
        Reset();
    }
}

void CConfig::RemoveComments(std::string& content)
{
    bool inComment = false;
    for (auto it = content.begin(); it != content.end(); )
    {
        if (*it == '#')
            inComment = true;
        else if (*it == '\n')
            inComment = false;
        
        if (inComment)
            it = content.erase(it);
        else
            ++it;
    }
}

void CConfig::RemoveWhitespaceKeepStringWhitespace(std::string& content)
{
    bool inString = false;
    auto it = std::remove_if(content.begin(), content.end(), [&](unsigned char c){
        if (c == '\"' && !inString)
            inString = true;
        else if (c =='\"' && inString)
            inString = false;

        if (inString) return false;
        if (std::isspace(c)) return true;
        return false;
    });

    content.erase(it, content.end());

    if (inString) throw std::runtime_error("Unterminated string");
}

void CConfig::ParseInto()
{
    Debug::Log("Opening config file.");

    /* Open the config file. */
    std::ifstream configFile(m_path, std::ios::in);
    if (configFile.bad()) throw std::runtime_error("Could not open config file.");

    /* Read in the whole config. */
    std::string content((std::istreambuf_iterator<char>(configFile)),
                        (std::istreambuf_iterator<char>()));

    RemoveComments(content);
    RemoveWhitespaceKeepStringWhitespace(content);
    
    CLexer lexer(content);
    CToken token = lexer.Next();
    while (token.Kind() != CToken::eEnd)
    {
        const char* kind = "";
        switch (token.Kind())
        {
            case CToken::eNumber: kind = "Number";break;
            case CToken::eIdentifier: kind = "Identifier";break;
            case CToken::eString: kind = "String";break;
            case CToken::eLeftCurly: kind = "LeftCurly";break;
            case CToken::eRightCurly: kind = "RightCurly";break;
            case CToken::eEqual: kind = "Equal";break;
            case CToken::eComma: kind = "Comma";break;
            case CToken::eEnd: kind = "End";break;
            case CToken::eUnexpected: kind = "Unexpected";break;
        }

        fmt::print("{} | {}\n", kind, token.Lexeme());

        token = lexer.Next();
    }
    //ParseValue(view);

    Debug::Log("Finished reading in config.");
}


//==============================================================================
// CLexerUtils
//==============================================================================

bool CLexerUtils::IsSpace(char c) noexcept
{
    switch (c)
    {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
    }
}

bool CLexerUtils::IsDigit(char c) noexcept
{
    switch (c)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
}

bool CLexerUtils::IsIdentifierChar(char c) noexcept
{
    switch (c)
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '_':
            return true;
        default:
            return false;
    }
}


//==============================================================================
// CLexer
//==============================================================================


CLexer::CLexer(const std::string& content) noexcept
    : m_content(content)
{
    m_it = content.begin();
}

CToken CLexer::Next() noexcept
{
    while (CLexerUtils::IsSpace(Peek())) Get();

    switch (Peek())
    {
        case '\0':
            return CToken(CToken::eEnd, std::string_view(m_it, m_it+1));
        default:
            return Atom(CToken::eUnexpected);
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            return Identifier();
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return Number();
        case '{':
            return Atom(CToken::eLeftCurly);
        case '}':
            return Atom(CToken::eRightCurly);
        case '=':
            return Atom(CToken::eEqual);
        case '"':
            return String();
        case ',':
            return Atom(CToken::eComma);
    }
}

CToken CLexer::Identifier() noexcept
{
    std::string::const_iterator start = m_it;
    Get();
    while (CLexerUtils::IsIdentifierChar(Peek())) Get();
    return CToken(CToken::eIdentifier, std::string_view{start, m_it});
}

CToken CLexer::String() noexcept
{
    std::string::const_iterator start = m_it;
    Get();
    while (Peek() != '\"' && Peek() != '\0') Get();
    Get();
    return CToken{CToken::eString, std::string_view{start, m_it}};
}

CToken CLexer::Number() noexcept
{
    std::string::const_iterator start = m_it;
    Get();
    while (CLexerUtils::IsDigit(Peek())) Get();
    return CToken(CToken::eNumber, std::string_view{start, m_it});
}

CToken CLexer::Atom(CToken::EKind kind) noexcept
{
    return CToken{kind, std::string_view{m_it++, m_it}};
}