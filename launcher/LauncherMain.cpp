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
#include "SharedFlags.h"
#include "SharedModule.h"
#include "SharedEngine.h"

/*
	Launches the engine.
	Currently can only launch the engine in one mode, but this is planned
	to also launch the engine directly into editor mode in the future.
*/

EngineExport_t *Engine;
EngineImport_t *Launcher;

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
	// Load the module interface for the engine module.
	Engine = (EngineExport_t*)pModule_LoadInterface(
		hEngineInstance,
		"./"MODULE_ENGINE,
		"Engine_Main",
		&Launcher);
	// Let us know if it failed to load.
	if(!Engine)
	{
		gWindow_MessageBox("Launcher",pError_Get());
		return -1;
	}
	// Also ensure that the engine version hasn't changed.
	else if (Engine->iVersion != ENGINE_VERSION)
	{
		gWindow_MessageBox("Launcher","Launcher is outdated, please rebuild!");

		pModule_Unload(hEngineInstance);
		return -1;
	}

	// Initialize.
	if (!Engine->Initialize(argc, argv, false))
	{
		gWindow_MessageBox("Launcher", "Failed to initialize engine!");

		pModule_Unload(hEngineInstance);
		return -1;
	}

	while (Engine->IsRunning())
		Engine->Loop();

	// Unload once the engine has stopped running.
	pModule_Unload(hEngineInstance);

	return -1;
}
