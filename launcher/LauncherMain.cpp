/*	Copyright (C) 2011-2015 OldTimes Software
*/

/*
	Launches the engine.
	Currently can only launch the engine in one mode, but this is planned
	to also launch the engine directly into editor mode in the future.
*/

// Platform Library
#include "platform_module.h"
#include "platform_window.h"
#include "platform_math.h"

// Shared Library
#include "SharedFlags.h"
#include "shared_module.h"
#include "shared_engine.h"

EngineExport_t	*Engine;
EngineImport_t	*Launcher;

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
	char sPath[PLATFORM_MAX_PATH];

	// Update the path to point to where we need it.
	sprintf_s(sPath, "./%s/%s", PATH_ENGINE, MODULE_ENGINE);

	// Load the module interface for the engine module.
	Engine = (EngineExport_t*)pModule_LoadInterface(
		hEngineInstance,
		sPath,	//"./"PATH_ENGINE"/"MODULE_ENGINE,
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

	// Because the loop is within the initialize function.
	pModule_Unload(hEngineInstance);

	return -1;
}
