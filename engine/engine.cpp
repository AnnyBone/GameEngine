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

unsigned int engine::Initialize(int argc, char **argv) {
    // todo

    char username[PL_MAX_USERNAME] = {};
    plGetUserName(username);
    host::username = username;

    host::path_fonts        = "fonts/";
    host::path_levels       = "levels/";
    host::path_materials    = "materials/";
    host::path_plugins      = "plugins/";
    host::path_screenshots  = "screenshots/";
    host::path_textures     = "materials/";
    host::path_shaders      = "shaders/";
    host::path_sounds       = "sounds/";

    return ENGINE_RESULT_SUCCESS;
}

void engine::Shutdown() {
    // todo
}

void engine::Loop() {
    // todo
}

/*  Version */

#include "engine_build.h"

unsigned int engine::GetVersion() {
    return ENGINE_VERSION_BUILD;
}

unsigned int engine::GetInterfaceVersion() {
    return 0;
}

const char *engine::GetVersionString() {
    return std::to_string(ENGINE_VERSION_BUILD).c_str();
}