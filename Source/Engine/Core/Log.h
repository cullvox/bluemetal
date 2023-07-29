#pragma once


#include "Precompiled.h"
#include "Export.h"
#include "Core/Platform.h"

#ifdef BLUEMETAL_SYSTEM_WINDOWS
    #define BLUEMETAL_SLASH '\\'
#else
    #define BLUEMETAL_SLASH '/'
#endif

namespace bl
{

enum class LogType
{
    eInfo,
    eWarning,
    eError,
    eFatal,
    eDebug,
};

class BLUEMETAL_API Logger
{
public:

    static Logger* getGlobal();

    Logger();
    ~Logger();
    
    static void vlog(LogType type, const char* libraryName, int line, const char* function, fmt::string_view format, fmt::format_args args) 
    {
        static const auto debug_style    = fmt::emphasis::bold | fg(fmt::color::dark_orange);
        static const auto error_style    = fmt::emphasis::bold | fg(fmt::color::dark_red);
        static const auto warn_style     = fmt::emphasis::bold | fg(fmt::color::yellow);
        static const auto info_style     = fg(fmt::color::antique_white);
        static const auto fatal_style    = fmt::emphasis::bold | fg(fmt::color::white) | bg(fmt::color::crimson);
        
#ifndef BLUEMETAL_DEBUG
        if (type == LogType::eDebug) return;
#endif

        fmt::text_style style;
        std::string type_name;

        switch (type)
        {
            case LogType::eInfo:      style = info_style; type_name = "INFO"; break;
            case LogType::eWarning:   style = warn_style; type_name = "WARNING"; break;
            case LogType::eError:     style = error_style; type_name = "ERROR"; break;
            case LogType::eDebug:     style = debug_style; type_name = "DEBUG"; break;
            case LogType::eFatal:     style = fatal_style; type_name = "FATAL"; break;
        }

        fmt::print(style, "{} | {}:{} | {:%Y-%m-%d-%H:%M:%S} | {}() - ", type_name, libraryName, line, fmt::localtime(std::chrono::system_clock::now()), function);
        fmt::vprint(fmt::format(style, "{}\n", format), args);

        if (type == LogType::eFatal)
            abort();
    }
    
    static constexpr const char* file_name(const char* path) {
        const char* file = path;
        while (*path) {
            if (*path++ == BLUEMETAL_SLASH) {
                file = path;
            }
        }
        return file;
    }


    template<typename TString, typename...TArgs>
    static void log(LogType type, const char* pLibraryName, int line, const char* pFunctionName, const TString& format, TArgs&&... args)
    {
        vlog(type, pLibraryName, line, pFunctionName, format, fmt::make_format_args(args...));
    }

    
};

} // namespace bl

#define BL_LOG(type, format, ...) bl::Logger::log(type, bl::Logger::file_name(__FILE__), __LINE__, __func__, FMT_STRING(format) __VA_OPT__(,) __VA_ARGS__);