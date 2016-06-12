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

// Platform Library
#include "platform.h"
#include "platform_library.h"

// Shared Library
#include "shared_flags.h"
#include "shared_engine.h"

/*
	Launches the engine.
	Currently can only launch the engine in one mode, but this is planned
	to also launch the engine directly into editor mode in the future.
*/

#define	LAUNCHER_LOG "launcher"

EngineExport_t *engine;
EngineImport_t *launcher;

PL_INSTANCE instance;

/*	TODO:
		List of things I want access to from the engine...
			Engine->GetBasePath()				(Returns current base path)
			Engine->CreateEntity()				(Returns an entity instance)
			Engine->LoadMap(file)				(Loads the specified level)
			Engine->CreateBrush(origin,size)	(Creates a brush)
			Engine->CreateWindow(x,y,w,h)		(Creates a new window... Need to handle different window instances somehow... Sigh)
*/

int main(int argc,char *argv[])
{
	plClearLog(LAUNCHER_LOG);
	plWriteLog(LAUNCHER_LOG, "Launcher (Interface Version %i)\n", ENGINE_VERSION);

	// Load the module interface for the engine module.
	engine = (EngineExport_t*)plLoadLibraryInterface(
		instance,
		"./" ENGINE_MODULE,
		"Engine_Main",
		&launcher);
	// Let us know if it failed to load.
	if (!engine)
	{
		std::string err = plGetError();

		plWriteLog(LAUNCHER_LOG, "Failed to load engine!\n%s", err.c_str());
		plMessageBox("Launcher", "%s", err.c_str());
		return -1;
	}
	// Also ensure that the engine version hasn't changed.
	else if (engine->iVersion != ENGINE_VERSION)
	{
		plWriteLog(LAUNCHER_LOG, "Launcher is outdated, please rebuild! (%i)\n", engine->iVersion);
		plMessageBox("Launcher", "Launcher is outdated, please rebuild!");
		plUnloadLibrary(instance);
		return -1;
	}

	// Initialize.
	if (!engine->Initialize(argc, argv, false))
	{
		plWriteLog(LAUNCHER_LOG, "Engine failed to initialize, check engine log!\n");
		plMessageBox("Launcher", "Failed to initialize engine!");
		plUnloadLibrary(instance);
		return -1;
	}

	while (engine->IsRunning())
		engine->Loop();

	// Unload once the engine has stopped running.
	plUnloadLibrary(instance);

	return -1;
}
