/*	Copyright (C) 2011-2016 OldTimes Software

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

#include "engine_base.h"

#include "video.h"
#include "EngineEditor.h"
#include "engine_client.h"

EngineGlobal_t g_state;

bool Engine_IsRunning(void)
{
	return g_hostinitialized;
}

char *Engine_GetBasePath(void)
{
	return host_parms.basepath;
}

char *Engine_GetMaterialPath(void)
{
	return g_state.path_materials;
}

char *Engine_GetVersion(void)
{
	return va("%i", ENGINE_VERSION_BUILD);
}

bool System_Main(int iArgumentCount,char *cArguments[], bool bEmbedded);
void System_Loop(void);

EngineImport_t g_launcher;
EngineExport_t exports;

pMODULE_EXPORT EngineExport_t *Engine_Main(EngineImport_t *mImport)
{
	// Imports
	g_launcher.iVersion = mImport->iVersion;
	g_launcher.PrintError = mImport->PrintError;
	g_launcher.PrintMessage = mImport->PrintMessage;
	g_launcher.PrintWarning = mImport->PrintWarning;

	// Exports
	exports.Initialize = System_Main;
	exports.Shutdown = Host_Shutdown;
	exports.IsRunning = Engine_IsRunning;
	exports.SetViewportSize = Video_SetViewportSize;
	exports.Loop = System_Loop;
	exports.iVersion = ENGINE_VERSION;

	// Client
	exports.ClientDisconnect = CL_Disconnect;
	exports.GetClientTime = Client_GetTime;
	exports.CreateClientEntity = CL_NewTempEntity;
	exports.CreateDynamicLight = Client_AllocDlight;

	// Server
	exports.ServerShutdown = Host_ShutdownServer;

	exports.GetBasePath = Engine_GetBasePath;
	exports.GetVersion = Engine_GetVersion;

	// Material
	exports.GetMaterialPath = Engine_GetMaterialPath;
	exports.LoadMaterial = Material_Load;
	exports.UnloadMaterial = Material_Clear;

	// Model
	exports.LoadModel = Mod_ForName;

	// Console
	exports.InsertConsoleCommand = Cbuf_InsertText;
	exports.RegisterConsoleVariable = Cvar_RegisterVariable;
	exports.SetConsoleVariable = Cvar_Set;
	exports.ResetConsoleVariable = Cvar_Reset;
	exports.GetConsoleVariableValue = Cvar_VariableValue;
	exports.GetConsoleVariableBoolValue = ConsoleVariable_GetBoolValue;
	exports.Print = Con_Printf;
	exports.PrintDev = Con_DPrintf;

	// Video
	exports.DrawPostFrame = Video_PostFrame;
	exports.DrawPreFrame = Video_PreFrame;
	exports.DrawEntity = Draw_Entity;
	exports.DrawFPS = Screen_DrawFPS;
	exports.DrawConsole = Screen_DrawConsole;
	exports.DrawGrid = Draw_Grid;
	exports.DrawString = Draw_String;
	exports.DrawLine = Draw_Line;
	exports.DrawGradientBackground = Draw_GradientBackground;
	exports.DrawSetCanvas = GL_SetCanvas;
	exports.DrawResetCanvas = Draw_ResetCanvas;
	exports.DrawMaterialSurface = Draw_MaterialSurface;

	// Material Editor
	exports.MaterialEditorInitialize = MaterialEditor_Initialize;

	return &exports;
}
