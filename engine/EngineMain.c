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
#include "EngineEditor.h"
#include "engine_client.h"

EngineGlobal_t Global;

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

char *Engine_GetVersion(void)
{
	return va("%i.%i.%i",
		ENGINE_VERSION_MAJOR,
		ENGINE_VERSION_MINOR,
		ENGINE_VERSION_BUILD);
}

bool System_Main(int iArgumentCount,char *cArguments[], bool bEmbedded);
void System_Loop(void);

EngineExport_t EngineExport;
EngineImport_t EngineLauncher;

pMODULE_EXPORT EngineExport_t *Engine_Main(EngineImport_t *mImport)
{
	// Imports
	EngineLauncher.iVersion = mImport->iVersion;
	EngineLauncher.PrintError = mImport->PrintError;
	EngineLauncher.PrintMessage = mImport->PrintMessage;
	EngineLauncher.PrintWarning = mImport->PrintWarning;

	// Exports
	EngineExport.Initialize = System_Main;
	EngineExport.Shutdown = Host_Shutdown;
	EngineExport.IsRunning = Engine_IsRunning;
	EngineExport.SetViewportSize = Video_SetViewportSize;
	EngineExport.Loop = System_Loop;
	EngineExport.iVersion = ENGINE_VERSION;

	// Client
	EngineExport.ClientDisconnect = CL_Disconnect;
	EngineExport.GetClientTime = Client_GetTime;
	EngineExport.CreateClientEntity = CL_NewTempEntity;
	EngineExport.CreateDynamicLight = Client_AllocDlight;

	// Server
	EngineExport.ServerShutdown = Host_ShutdownServer;

	EngineExport.GetBasePath = Engine_GetBasePath;
	EngineExport.GetVersion = Engine_GetVersion;

	// Material
	EngineExport.GetMaterialPath = Engine_GetMaterialPath;
	EngineExport.LoadMaterial = Material_Load;
	EngineExport.UnloadMaterial = Material_Clear;

	// Model
	EngineExport.LoadModel = Mod_ForName;

	// Console
	EngineExport.InsertConsoleCommand = Cbuf_InsertText;
	EngineExport.RegisterConsoleVariable = Cvar_RegisterVariable;
	EngineExport.SetConsoleVariable = Cvar_Set;
	EngineExport.ResetConsoleVariable = Cvar_Reset;
	EngineExport.Print = Con_Printf;
	EngineExport.PrintDev = Con_DPrintf;

	// Video
	EngineExport.Video_PostFrame = Video_PostFrame;
	EngineExport.Video_PreFrame = Video_PreFrame;
	EngineExport.DrawEntity = Draw_Entity;
	EngineExport.DrawFPS = Screen_DrawFPS;
	EngineExport.DrawConsole = Screen_DrawConsole;
	EngineExport.DrawString = Draw_String;
	EngineExport.DrawLine = Draw_Line;
	EngineExport.DrawGradientBackground = Draw_GradientBackground;
	EngineExport.DrawSetCanvas = GL_SetCanvas;
	EngineExport.DrawResetCanvas = Draw_ResetCanvas;

	// Material Editor
	EngineExport.MaterialEditorInitialize = MaterialEditor_Initialize;

	return &EngineExport;
}
