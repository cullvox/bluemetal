
#ifndef BLUEMETAL_API_H
#define BLUEMETAL_API_H

#ifdef BLUEMETAL_STATIC_DEFINE
#  define BLUEMETAL_API
#  define BLUEMETAL_NO_EXPORT
#else
#  ifndef BLUEMETAL_API
#    ifdef BlueMetal_EXPORTS
        /* We are building this library */
#      define BLUEMETAL_API 
#    else
        /* We are using this library */
#      define BLUEMETAL_API 
#    endif
#  endif

#  ifndef BLUEMETAL_NO_EXPORT
#    define BLUEMETAL_NO_EXPORT 
#  endif
#endif

#ifndef BLUEMETAL_DEPRECATED
#  define BLUEMETAL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef BLUEMETAL_DEPRECATED_EXPORT
#  define BLUEMETAL_DEPRECATED_EXPORT BLUEMETAL_API BLUEMETAL_DEPRECATED
#endif

#ifndef BLUEMETAL_DEPRECATED_NO_EXPORT
#  define BLUEMETAL_DEPRECATED_NO_EXPORT BLUEMETAL_NO_EXPORT BLUEMETAL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLUEMETAL_NO_DEPRECATED
#    define BLUEMETAL_NO_DEPRECATED
#  endif
#endif

#endif /* BLUEMETAL_API_H */
