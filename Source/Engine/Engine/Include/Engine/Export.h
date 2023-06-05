
#ifndef BLUEMETAL_ENGINE_API_H
#define BLUEMETAL_ENGINE_API_H

#ifdef BLUEMETAL_ENGINE_STATIC_DEFINE
#  define BLUEMETAL_ENGINE_API
#  define BLUEMETAL_ENGINE_NO_EXPORT
#else
#  ifndef BLUEMETAL_ENGINE_API
#    ifdef BLEngine_EXPORTS
        /* We are building this library */
#      define BLUEMETAL_ENGINE_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define BLUEMETAL_ENGINE_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef BLUEMETAL_ENGINE_NO_EXPORT
#    define BLUEMETAL_ENGINE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef BLUEMETAL_DEPRECATED
#  define BLUEMETAL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef BLUEMETAL_DEPRECATED_EXPORT
#  define BLUEMETAL_DEPRECATED_EXPORT BLUEMETAL_ENGINE_API BLUEMETAL_DEPRECATED
#endif

#ifndef BLUEMETAL_DEPRECATED_NO_EXPORT
#  define BLUEMETAL_DEPRECATED_NO_EXPORT BLUEMETAL_ENGINE_NO_EXPORT BLUEMETAL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLUEMETAL_ENGINE_NO_DEPRECATED
#    define BLUEMETAL_ENGINE_NO_DEPRECATED
#  endif
#endif

#endif /* BLUEMETAL_ENGINE_API_H */
