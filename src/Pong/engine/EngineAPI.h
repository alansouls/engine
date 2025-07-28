#pragma once

// Engine API export/import macros for DLL support
#ifdef WINDOWS
    #ifdef SSGE_ENGINE_EXPORTS
        #define SSGE_API __declspec(dllexport)
    #else
        #define SSGE_API __declspec(dllimport)
    #endif
#else
    #define SSGE_API
#endif

// Additional macros for template instantiation control
#ifdef WINDOWS
    #define SSGE_EXTERN_TEMPLATE extern template
    #define SSGE_TEMPLATE_INSTANTIATION template
#else
    #define SSGE_EXTERN_TEMPLATE
    #define SSGE_TEMPLATE_INSTANTIATION
#endif
