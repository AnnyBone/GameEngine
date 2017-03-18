
#include "engine.h"

#include <SDL2/SDL.h>

double System_GetDoubleTime(void) {
    return SDL_GetTicks() / 1000;
}

void System_Error(const char *error, ...) {
#ifdef _DEBUG
    assert(error);
#endif

    static bool in_sys_error[4] = { false, false, false };
    if(!in_sys_error[3]) {
        in_sys_error[3] = true;
    }

    char text[1024] = { 0 };

    va_list argptr;
    va_start(argptr,error);
    vsprintf(text,error,argptr);
    va_end(argptr);

    plWriteLog(ENGINE_LOG, "system_error: %s", text);

    if(!in_sys_error[0]) {
        in_sys_error[0] = true;

        plMessageBox("Fatal Error", text);
    } else {
        plMessageBox("Double Fatal Error", text);
    }

    if(!in_sys_error[1]) {
        in_sys_error[1] = true;

        Engine_Shutdown();
    }
}