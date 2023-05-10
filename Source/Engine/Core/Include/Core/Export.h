
#ifndef BLUEMETAL_CORE_API_H
#define BLUEMETAL_CORE_API_H

#ifdef BLUEMETAL_CORE_STATIC_DEFINE
#  define BLUEMETAL_CORE_API
#  define BLUEMETAL_CORE_NO_EXPORT
#else
#  ifndef BLUEMETAL_CORE_API
#    ifdef BloodLustCore_EXPORTS
        /* We are building this library */
#      define BLUEMETAL_CORE_API 
#    else
        /* We are using this library */
#      define BLUEMETAL_CORE_API 
#    endif
#  endif

#  ifndef BLUEMETAL_CORE_NO_EXPORT
#    define BLUEMETAL_CORE_NO_EXPORT 
#  endif
#endif

#ifndef BLUEMETAL_DEPRECATED
#  define BLUEMETAL_DEPRECATED __declspec(deprecated)
#endif

#ifndef BLUEMETAL_DEPRECATED_EXPORT
#  define BLUEMETAL_DEPRECATED_EXPORT BLUEMETAL_CORE_API BLUEMETAL_DEPRECATED
#endif

#ifndef BLUEMETAL_DEPRECATED_NO_EXPORT
#  define BLUEMETAL_DEPRECATED_NO_EXPORT BLUEMETAL_CORE_NO_EXPORT BLUEMETAL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLUEMETAL_CORE_NO_DEPRECATED
#    define BLUEMETAL_CORE_NO_DEPRECATED
#  endif
#endif

#endif /* BLUEMETAL_CORE_API_H */
