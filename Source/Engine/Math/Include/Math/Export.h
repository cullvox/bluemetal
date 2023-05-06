
#ifndef BLOODLUST_MATH_API_H
#define BLOODLUST_MATH_API_H

#ifdef BLOODLUST_MATH_STATIC_DEFINE
#  define BLOODLUST_MATH_API
#  define BLOODLUST_MATH_NO_EXPORT
#else
#  ifndef BLOODLUST_MATH_API
#    ifdef BloodLustMath_EXPORTS
        /* We are building this library */
#      define BLOODLUST_MATH_API 
#    else
        /* We are using this library */
#      define BLOODLUST_MATH_API 
#    endif
#  endif

#  ifndef BLOODLUST_MATH_NO_EXPORT
#    define BLOODLUST_MATH_NO_EXPORT 
#  endif
#endif

#ifndef BLOODLUST_DEPRECATED
#  define BLOODLUST_DEPRECATED __declspec(deprecated)
#endif

#ifndef BLOODLUST_DEPRECATED_EXPORT
#  define BLOODLUST_DEPRECATED_EXPORT BLOODLUST_MATH_API BLOODLUST_DEPRECATED
#endif

#ifndef BLOODLUST_DEPRECATED_NO_EXPORT
#  define BLOODLUST_DEPRECATED_NO_EXPORT BLOODLUST_MATH_NO_EXPORT BLOODLUST_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLOODLUST_MATH_NO_DEPRECATED
#    define BLOODLUST_MATH_NO_DEPRECATED
#  endif
#endif

#endif /* BLOODLUST_MATH_API_H */
