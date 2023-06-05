
#ifndef BLUEMETAL_MATH_API_H
#define BLUEMETAL_MATH_API_H

#ifdef BLUEMETAL_MATH_STATIC_DEFINE
#  define BLUEMETAL_MATH_API
#  define BLUEMETAL_MATH_NO_EXPORT
#else
#  ifndef BLUEMETAL_MATH_API
#    ifdef BLMath_EXPORTS
        /* We are building this library */
#      define BLUEMETAL_MATH_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define BLUEMETAL_MATH_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef BLUEMETAL_MATH_NO_EXPORT
#    define BLUEMETAL_MATH_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef BLUEMETAL_DEPRECATED
#  define BLUEMETAL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef BLUEMETAL_DEPRECATED_EXPORT
#  define BLUEMETAL_DEPRECATED_EXPORT BLUEMETAL_MATH_API BLUEMETAL_DEPRECATED
#endif

#ifndef BLUEMETAL_DEPRECATED_NO_EXPORT
#  define BLUEMETAL_DEPRECATED_NO_EXPORT BLUEMETAL_MATH_NO_EXPORT BLUEMETAL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLUEMETAL_MATH_NO_DEPRECATED
#    define BLUEMETAL_MATH_NO_DEPRECATED
#  endif
#endif

#endif /* BLUEMETAL_MATH_API_H */
