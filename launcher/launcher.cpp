/*	Copyright (C) 2011-2015 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// Platform Library
#include "platform_module.h"
#include "platform_window.h"
#include "platform_math.h"

// Shared Library
#include "shared_flags.h"
#include "SharedModule.h"
#include "shared_engine.h"

/*
	Launches the engine.
	Currently can only launch the engine in one mode, but this is planned
	to also launch the engine directly into editor mode in the future.
*/

#define	LAUNCHER_LOG "launcher"

EngineExport_t *engine;
EngineImport_t *launcher;

pINSTANCE hEngineInstance;

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
	pLog_Clear(LAUNCHER_LOG);
	pLog_Write(LAUNCHER_LOG, "Launcher (Interface Version %i)\n", ENGINE_VERSION);

	// Load the module interface for the engine module.
	engine = (EngineExport_t*)pModule_LoadInterface(
		hEngineInstance,
		"./" MODULE_ENGINE,
		"Engine_Main",
		&launcher);
	// Let us know if it failed to load.
	if (!engine)
	{
		pLog_Write(LAUNCHER_LOG, "Failed to load engine!\n%s",pError_Get());
		pWindow_MessageBox("Launcher", pError_Get());
		return -1;
	}
	// Also ensure that the engine version hasn't changed.
	else if (engine->iVersion != ENGINE_VERSION)
	{
		pLog_Write(LAUNCHER_LOG, "Launcher is outdated, please rebuild! (%i)\n", engine->iVersion);
		pWindow_MessageBox("Launcher", "Launcher is outdated, please rebuild!");
		pModule_Unload(hEngineInstance);
		return -1;
	}

	// Initialize.
	if (!engine->Initialize(argc, argv, false))
	{
		pLog_Write(LAUNCHER_LOG, "Engine failed to initialize, check engine log!\n");
		pWindow_MessageBox("Launcher", "Failed to initialize engine!");
		pModule_Unload(hEngineInstance);
		return -1;
	}

	while (engine->IsRunning())
		engine->Loop();

	// Unload once the engine has stopped running.
	pModule_Unload(hEngineInstance);

	return -1;
}
