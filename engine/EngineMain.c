/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoMaterial.h"
#include "EngineEditor.h"

#include "platform_module.h"

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
	eExport.Shutdown = Host_Shutdown;
	eExport.IsRunning = Engine_IsRunning;
	eExport.SetViewportSize = Video_SetViewportSize;
	eExport.Loop = System_Loop;
	eExport.iVersion = ENGINE_VERSION;

	eExport.GetBasePath = Engine_GetBasePath;
	eExport.GetMaterialPath = Engine_GetMaterialPath;
	eExport.GetVersion = Engine_GetVersion;

	eExport.LoadMaterial = Material_Load;
	eExport.UnloadMaterial = Material_Clear;

	// Console
	eExport.InsertConsoleCommand = Cbuf_InsertText;
	eExport.RegisterConsoleVariable = Cvar_RegisterVariable;
	eExport.SetConsoleVariable = Cvar_Set;
	eExport.ResetConsoleVariable = Cvar_Reset;
	eExport.Print = Con_Printf;
	eExport.PrintDev = Con_DPrintf;

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
