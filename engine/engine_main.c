/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "engine_main.h"

#include "platform_module.h"

#include "video.h"
#include "EngineVideoMaterial.h"
#include "engine_videoshadow.h"
#include "engine_editor.h"

bool Engine_IsRunning(void)
{
	return bHostInitialized;
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
	// Imports
	Launcher.iVersion = mImport->iVersion;
	Launcher.PrintError = mImport->PrintError;
	Launcher.PrintMessage = mImport->PrintMessage;
	Launcher.PrintWarning = mImport->PrintWarning;

	// Exports
	eExport.Initialize = System_Main;
	eExport.IsRunning = Engine_IsRunning;
	eExport.SetViewportSize = Video_SetViewportSize;
	eExport.Loop = System_Loop;
	eExport.iVersion = ENGINE_VERSION;

	eExport.GetBasePath = Engine_GetBasePath;
	eExport.GetMaterialPath = Engine_GetMaterialPath;
	eExport.GetVersion = Engine_GetVersion;

	eExport.LoadMaterial = Material_Load;

	// Video
	eExport.Video_PostFrame = Video_PostFrame;
	eExport.Video_PreFrame = Video_PreFrame;
	eExport.DrawFPS = Screen_DrawFPS;
	eExport.DrawConsole = Screen_DrawConsole;
	eExport.DrawString = Draw_String;
	eExport.DrawLine = Draw_Line;
	eExport.DrawModel = MaterialEditor_Draw;
	eExport.ResetCanvas = Draw_ResetCanvas;

	// Material Editor
	eExport.MaterialEditorInitialize = MaterialEditor_Initialize;
	eExport.MaterialEditorDisplay = MaterialEditor_Display;

	return &eExport;
}
