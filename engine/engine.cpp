
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