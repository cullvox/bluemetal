
#ifndef IMGUI_EXPORT_H
#define IMGUI_EXPORT_H

#ifdef IMGUI_STATIC_DEFINE
#  define IMGUI_EXPORT
#  define IMGUI_NO_EXPORT
#else
#  ifndef IMGUI_EXPORT
#    ifdef ImGui_EXPORTS
        /* We are building this library */
#      define IMGUI_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define IMGUI_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef IMGUI_NO_EXPORT
#    define IMGUI_NO_EXPORT 
#  endif
#endif

#ifndef IMGUI_DEPRECATED
#  define IMGUI_DEPRECATED __declspec(deprecated)
#endif

#ifndef IMGUI_DEPRECATED_EXPORT
#  define IMGUI_DEPRECATED_EXPORT IMGUI_EXPORT IMGUI_DEPRECATED
#endif

#ifndef IMGUI_DEPRECATED_NO_EXPORT
#  define IMGUI_DEPRECATED_NO_EXPORT IMGUI_NO_EXPORT IMGUI_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef IMGUI_NO_DEPRECATED
#    define IMGUI_NO_DEPRECATED
#  endif
#endif

#endif /* IMGUI_EXPORT_H */
