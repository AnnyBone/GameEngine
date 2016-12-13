/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "platform_library.h"
#include "platform_log.h"
#include "platform_window.h"

#include "shared_engine.h"

/*  Launcher    */

#define    LAUNCHER_LOG "launcher"

int main(int argc, char *argv[]) {
    plClearLog(LAUNCHER_LOG);
    //plWriteLog(LAUNCHER_LOG, "Launcher (Interface Version %i)\n", ENGINE_VERSION_INTERFACE);

    // Initialize.
    if (engine::Initialize(argc, argv) != ENGINE_RESULT_SUCCESS) {
        plWriteLog(LAUNCHER_LOG, "Engine failed to initialize, check engine log!\n");
        plMessageBox("Launcher", "Failed to initialize engine!");
        return -1;
    }

    while (engine::IsActive())
        engine::Loop();

    return 0;
}
