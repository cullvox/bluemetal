
#ifndef BLOODLUST_CORE_API_H
#define BLOODLUST_CORE_API_H

#ifdef BLOODLUST_CORE_STATIC_DEFINE
#  define BLOODLUST_CORE_API
#  define BLOODLUST_CORE_NO_EXPORT
#else
#  ifndef BLOODLUST_CORE_API
#    ifdef BloodLustCore_EXPORTS
        /* We are building this library */
#      define BLOODLUST_CORE_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define BLOODLUST_CORE_API __declspec(dllimport)
#    endif
#  endif

#  ifndef BLOODLUST_CORE_NO_EXPORT
#    define BLOODLUST_CORE_NO_EXPORT 
#  endif
#endif

#ifndef BLOODLUST_DEPRECATED
#  define BLOODLUST_DEPRECATED __declspec(deprecated)
#endif

#ifndef BLOODLUST_DEPRECATED_EXPORT
#  define BLOODLUST_DEPRECATED_EXPORT BLOODLUST_CORE_API BLOODLUST_DEPRECATED
#endif

#ifndef BLOODLUST_DEPRECATED_NO_EXPORT
#  define BLOODLUST_DEPRECATED_NO_EXPORT BLOODLUST_CORE_NO_EXPORT BLOODLUST_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLOODLUST_CORE_NO_DEPRECATED
#    define BLOODLUST_CORE_NO_DEPRECATED
#  endif
#endif

#endif /* BLOODLUST_CORE_API_H */
