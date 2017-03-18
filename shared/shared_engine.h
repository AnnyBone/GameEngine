/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#pragma once

// Functions exported from the engine.
typedef struct EngineExport {

} EngineExport;

// Functions imported by the engine.
typedef struct EngineImport {
    int version;

    void (*PrintMessage)(const char *text);
    void (*PrintWarning)(const char *text);
    void (*PrintError)(const char *text);
} EngineImport;

/////////////////////////////////////////////////////////////////////////////////

#define ENGINE_MODULE               "engine"
#define ENGINE_VERSION_INTERFACE    (sizeof(EngineImport) + sizeof(EngineExport))

#if defined(ENGINE_INTERNAL)
#	define ENGINE_EXPORT PL_EXPORT
#else
#	define ENGINE_EXPORT
#endif

ENGINE_EXPORT bool Engine_Initialize(int argc, char **argv);

ENGINE_EXPORT void Engine_Loop(void);

ENGINE_EXPORT unsigned int Engine_GetVersion(void);
ENGINE_EXPORT unsigned int Engine_GetInterfaceVersion(void);

ENGINE_EXPORT void Engine_Shutdown(void);
