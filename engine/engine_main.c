/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "engine_main.h"

#include "platform_module.h"

#include "video.h"
#include "EngineVideoMaterial.h"
#include "engine_editor.h"

EngineExport_t	eExport;
EngineImport_t	Launcher;

bool Engine_IsRunning(void)
{
	return true;
}

char *Engine_GetBasePath(void)
{
	return host_parms.cBasePath;
}

char *Engine_GetMaterialPath(void)
{
	return Global.cMaterialPath;
}

char* Engine_GetVersion(void)
{
	return va("%i.%i.%i",
		ENGINE_VERSION_MAJOR,
		ENGINE_VERSION_MINOR,
		ENGINE_VERSION_BUILD);
}

bool System_Main(int iArgumentCount,char *cArguments[], bool bEmbedded);
void System_Loop(void);

pMODULE_EXPORT EngineExport_t *Engine_Main(EngineImport_t *mImport)
{
	Launcher.iVersion	= mImport->iVersion;

	eExport.Initialize = System_Main;
	eExport.IsRunning = Engine_IsRunning;
	eExport.SetViewportSize = Video_SetViewportSize;
	eExport.Loop = System_Loop;
	eExport.iVersion = ENGINE_VERSION;

	eExport.GetBasePath = Engine_GetBasePath;
	eExport.GetMaterialPath = Engine_GetMaterialPath;
	eExport.GetVersion = Engine_GetVersion;

	eExport.LoadMaterial = Material_Load;

	// Material Editor
	eExport.MaterialEditorInitialize = MaterialEditor_Initialize;
	eExport.MaterialEditorDisplay = MaterialEditor_Display;

	return &eExport;
}
