
#pragma once

#if defined(ENGINE)
#   define ENGINE_FUNCTION  PL_MODULE_EXPORT
#else
#   define ENGINE_FUNCTION
#endif

enum {
    ENGINE_RESULT_SUCCESS,  // Everything worked out fine!
};

namespace engine {
    ENGINE_FUNCTION unsigned int Initialize(int argc, char *argv[]);

    ENGINE_FUNCTION void Shutdown();
    ENGINE_FUNCTION void Loop();

    ENGINE_FUNCTION bool IsActive();

    ENGINE_FUNCTION unsigned int GetVersion();
    ENGINE_FUNCTION unsigned int GetInterfaceVersion();
    ENGINE_FUNCTION const char *GetVersionString();
}