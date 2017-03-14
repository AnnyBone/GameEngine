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

#include "engine.h"

#include "engine_system.h"

using namespace xenon;

bool engine::Initialize(int argc, char **argv) {

    return true;
}

void engine::Shutdown() {}

void engine::Loop() {
    static double time, newtime, oldtime = 0;

    newtime = System_DoubleTime();
    time = newtime - oldtime;

    oldtime = newtime;
}

// Version Information

#include "engine_build.h"

uint32_t engine::GetVersion() {
    return ENGINE_VERSION_BUILD;
}

const char *engine::GetVersionString() {
    return std::to_string(ENGINE_VERSION_BUILD).c_str();
}

int32_t engine::GetInterfaceVersion() {
    return ENGINE_VERSION_INTERFACE;
}
