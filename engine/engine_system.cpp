
#include "engine.h"

#include "engine_system.h"

#include <SDL2/SDL.h>

double System_DoubleTime(void) {
    return SDL_GetTicks() / 1000;
}
