#pragma once

#include <stdexcept>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

enum class EConfigType
{
    eInteger,
    eFloat,
    eBoolean,
    eString
};

struct SConfigValue
{
    std::string name;
    EConfigType type;
    bool b;
    long i;
    float f;
    std::string s;
};

class CToken
{
public:
    enum EKind
    {
        eNumber,
        eIdentifier,
        eString,
        eLeftCurly,
        eRightCurly,
        eEqual,
        eComma,
        eEnd,
        eUnexpected,
    };

public:
    inline CToken(EKind kind, const std::string_view lexeme) noexcept
        : m_kind(kind), m_lexeme(lexeme) {}
    
    inline CToken& operator=(const CToken& other) = default;

    inline EKind            Kind() const noexcept { return m_kind; }
    inline std::string_view Lexeme() const noexcept { return m_lexeme; }
    inline void             Lexeme(const std::string_view& lexeme) noexcept { m_lexeme = lexeme; } 

private:
    EKind m_kind;
    std::string_view m_lexeme;
};

class CLexerUtils
{
public:
    static bool IsSpace(char c) noexcept;
    static bool IsDigit(char c) noexcept;
    static bool IsIdentifierChar(char c) noexcept;
};

class CLexer {
public:
    CLexer(const std::string& content) noexcept;

    CToken Next() noexcept;

private:
    CToken Identifier() noexcept;
    CToken String() noexcept;
    CToken Number() noexcept;
    CToken Atom(CToken::EKind kind) noexcept;
    inline char Peek() const noexcept { return *m_it; }
    inline char Get() noexcept { return *m_it++; }

    const std::string& m_content;
    std::string::const_iterator m_it;
};

class CConfig;
class IConfigurable
{
    CConfig *const m_pConfig = nullptr;
public:
    IConfigurable() = default;
    IConfigurable(CConfig *const); /* Automatic subscription/unsubscription */
    ~IConfigurable();

    virtual void                        OnConfigReceive(const std::vector<SConfigValue>&) { }
    virtual void                        OnConfigPreSave() { }
    virtual void                        OnConfigChanged(const SConfigValue&) { }
//  virtual std::vector<SConfigValue>   OnConfigRequestDefaults() { return {}; }
};

class CConfig
{
public:
    static inline const std::string DEFAULT_CONFIG_PATH = "config/config.nwdl";
    static const std::string DEFAULT_CONFIG;

    CConfig(const std::string& configPath = DEFAULT_CONFIG_PATH);
    ~CConfig();

    int                 GetIntValue(const std::string&) const;
    float               GetFloatValue(const std::string&) const;
    bool                GetBooleanValue(const std::string&) const;
    const std::string&  GetStringValue(const std::string&) const;
    void                SetIntValue(const std::string&, int);
    void                SetFloatValue(const std::string&, float);
    void                SetBooleanValue(const std::string&, bool);
    void                SetStringValue(const std::string&, const std::string&);
    void                Reset();
    
    void                SubscribeEvents(IConfigurable const*);
    void                UnsubscribeEvents(IConfigurable const*);
private:
    

    void                                EnsureDirectoriesExist();
    void                                ResetIfConfigDoesNotExist();
    void                                ParseInto();
    void                                RemoveComments(std::string&);
    void                                RemoveWhitespaceKeepStringWhitespace(std::string&);
    std::string_view                    TokenizeGroup(std::string_view&);
    
    void                                NotifySubscribers();

    std::string                         m_path = DEFAULT_CONFIG_PATH;
    std::vector<SConfigValue>           m_values;
    std::vector<const IConfigurable*>   m_subscribers;
};

inline std::unique_ptr<CConfig> g_pConfig;
