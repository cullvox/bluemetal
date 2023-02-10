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
    int i;
    float f;
    std::string s;
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
    class CExceptConfigParserError : public std::runtime_error
    {
    public:
        CExceptConfigParserError(const char* what, int character) noexcept
            : std::runtime_error(what), m_character(character)
        {
        }
        inline int character() const noexcept { return m_character; }
    private:
        int m_character;
    };

    struct SParseResult
    {
        bool hasValue;
        SConfigValue value;
    };

    struct SParseContext
    {
        std::string group;
    };

    enum EEqualType
    {
        eGroup,
        eValue
    };

    void                                EnsureDirectoriesExist();
    void                                ResetIfConfigDoesNotExist();
    void                                ParseInto();
    void                                RemoveComments(std::string&);
    void                                RemoveWhitespaceKeepStringWhitespace(std::string&);
    std::string_view                    ParseName(std::string_view&);
    void                                ParseValue(std::string_view&);
    void                                ParseGroup(std::string_view&);
    void                                Parse(const std::string&);
    void                                NotifySubscribers();

    std::string                         m_path = DEFAULT_CONFIG_PATH;
    std::vector<SConfigValue>           m_values;
    std::vector<const IConfigurable*>   m_subscribers;
};

inline std::unique_ptr<CConfig> g_pConfig;
