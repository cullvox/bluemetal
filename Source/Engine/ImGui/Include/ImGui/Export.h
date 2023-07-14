
#ifndef BLUEMETAL_IMGUI_API_H
#define BLUEMETAL_IMGUI_API_H

#ifdef BLUEMETAL_IMGUI_STATIC_DEFINE
#  define BLUEMETAL_IMGUI_API
#  define BLUEMETAL_IMGUI_NO_EXPORT
#else
#  ifndef BLUEMETAL_IMGUI_API
#    ifdef BLImGui_EXPORTS
        /* We are building this library */
#      define BLUEMETAL_IMGUI_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define BLUEMETAL_IMGUI_API __declspec(dllimport)
#    endif
#  endif

#  ifndef BLUEMETAL_IMGUI_NO_EXPORT
#    define BLUEMETAL_IMGUI_NO_EXPORT 
#  endif
#endif

#ifndef BLUEMETAL_DEPRECATED
#  define BLUEMETAL_DEPRECATED __declspec(deprecated)
#endif

#ifndef BLUEMETAL_DEPRECATED_EXPORT
#  define BLUEMETAL_DEPRECATED_EXPORT BLUEMETAL_IMGUI_API BLUEMETAL_DEPRECATED
#endif

#ifndef BLUEMETAL_DEPRECATED_NO_EXPORT
#  define BLUEMETAL_DEPRECATED_NO_EXPORT BLUEMETAL_IMGUI_NO_EXPORT BLUEMETAL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BLUEMETAL_IMGUI_NO_DEPRECATED
#    define BLUEMETAL_IMGUI_NO_DEPRECATED
#  endif
#endif

#endif /* BLUEMETAL_IMGUI_API_H */
