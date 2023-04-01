
#ifndef BLOODLUST_CONFIG_API_H
#define BLOODLUST_CONFIG_API_H

#ifdef BLOODLUST_CONFIG_STATIC_DEFINE
#  define BLOODLUST_CONFIG_API
#  define BLOODLUST_CONFIG_NO_EXPORT
#else
#  ifndef BLOODLUST_CONFIG_API
#    ifdef BloodLustConfig_EXPORTS
        /* We are building this library */
#      define BLOODLUST_CONFIG_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define BLOODLUST_CONFIG_API __declspec(dllimport)
#    endif
#  endif

#  ifndef BLOODLUST_CONFIG_NO_EXPORT
#    define BLOODLUST_CONFIG_NO_EXPORT 
#  endif
#endif

#ifndef BLOODLUST_DEPRECATED
#  define BLOODLUST_DEPRECATED __declspec(deprecated)
#endif

#ifndef BLOODLUST_DEPRECATED_EXPORT
#  define BLOODLUST_DEPRECATED_EXPORT BLOODLUST_CONFIG_API BLOODLUST_DEPRECATED
#endif

#ifndef BLOODLUST_DEPRECATED_NO_EXPORT
#  define BLOODLUST_DEPRECATED_NO_EXPORT BLOODLUST_CONFIG_NO_EXPORT BLOODLUST_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLOODLUST_CONFIG_NO_DEPRECATED
#    define BLOODLUST_CONFIG_NO_DEPRECATED
#  endif
#endif

#endif /* BLOODLUST_CONFIG_API_H */
