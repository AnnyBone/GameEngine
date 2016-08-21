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
#include "platform_library.h"
#include "platform_log.h"
#include "platform_window.h"

#include "shared_engine.h"

/*
	Launches the engine.
	Currently can only launch the engine in one mode, but this is planned
	to also launch the engine directly into editor mode in the future.
*/

#define	LAUNCHER_LOG "launcher"

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
	//plWriteLog(LAUNCHER_LOG, "Launcher (Interface Version %i)\n", ENGINE_VERSION_INTERFACE);

	// Initialize.
	if (xenon::Initialize(argc, argv) != PL_RESULT_SUCCESS)
	{
		plWriteLog(LAUNCHER_LOG, "Engine failed to initialize, check engine log!\n");
		plMessageBox("Launcher", "Failed to initialize engine!");
		return -1;
	}

	while (xenon::IsRunning())
		xenon::Loop();

	return -1;
}
