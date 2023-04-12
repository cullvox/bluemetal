#pragma once

#include "Core/Precompiled.hpp"
#include "Core/Export.h"

enum class blLogType
{
    eInfo,
    eWarning,
    eError,
    eFatal,
    eDebug,
};

class BLOODLUST_CORE_API blLogger
{
public:

    static blLogger* getGlobal();

    blLogger();
    ~blLogger();

    template<typename... TArgs>
    constexpr static void blLogImpl(blLogType type, const char* libraryName, int line, const char* function, const fmt::format_string<TArgs...>& format, TArgs&&... targs) 
    {
        static const auto debug_style = fmt::emphasis::bold | fg(fmt::color::dark_orange);
        static const auto error_style = fmt::emphasis::bold | fg(fmt::color::dark_red);
        static const auto warn_style = fmt::emphasis::bold | fg(fmt::color::yellow);
        static const auto info_style = fg(fmt::color::antique_white);
        static const auto fatal_style = fmt::emphasis::bold | fg(fmt::color::white) | bg(fmt::color::crimson);
        
#ifndef BLOODLUST_DEBUG
        if (type == blLogType::eDebug)
            return;
#endif

        const auto args = fmt::make_format_args(targs...);
        fmt::text_style style;
        std::string type_name;

        switch (type)
        {
            case blLogType::eInfo: style = info_style; type_name = "INFO"; break;
            case blLogType::eWarning: style = warn_style; type_name = "WARNING"; break;
            case blLogType::eError: style = error_style; type_name = "ERROR"; break;
            case blLogType::eDebug: style = debug_style; type_name = "DEBUG"; break;
            case blLogType::eFatal: style = fatal_style; type_name = "FATAL"; break;
        }

        fmt::print(style, "{} | {} | {:%Y-%m-%d-%H:%M:%S} | {}():{} - ", type_name, libraryName, fmt::localtime(std::chrono::system_clock::now()), function, line);
        fmt::vprint(fmt::format(style, "{}", format), args);
        fmt::print("\n");
    }
};

#define BL_LOG(type, format, ...) \
    blLogger::blLogImpl(type, BLOODLUST_LIBRARY_NAME, __LINE__, __func__, format, __VA_ARGS__);