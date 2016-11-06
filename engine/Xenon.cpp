/*
Copyright (C) 2011-2016 OldTimes Software

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

#include "platform_log.h"

#include "engine_base.h"

#include "video.h"
#include "EngineEditor.h"
#include "engine_client.h"

XGlobal g_state;

double oldtime = 0;

PLresult xenon::Initialize(int argc, char *argv[])
{
	XParameters	parms;
	parms.memsize	= 0x8000000;	// 128MB
	parms.basedir	= ".";
	parms.cachedir	= NULL;
	parms.argc		= argc;
	parms.argv		= argv;

	COM_InitArgv(parms.argc, parms.argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	g_state.embedded = (COM_CheckParm("-embedded") != 0);

	if (COM_CheckParm("-heapsize"))
	{
		int i = COM_CheckParm("-heapsize") + 1;
		if (i < com_argc) parms.memsize = atoi(com_argv[i]) * 1024;
	}

	parms.membase = malloc(parms.memsize);
	if (!parms.membase)
	{
		plWriteLog(ENGINE_LOG, "Not enough memory free (%imb)!\nCheck disk space.\n", parms.memsize);
		return PL_RESULT_MEMORYALLOC;
	}

	Host_Initialize(&parms);

	oldtime = System_DoubleTime();

	return PL_RESULT_SUCCESS;
}

void xenon::Shutdown()
{

}

void xenon::Loop()
{
	static PLdouble time, newtime;

	newtime = System_DoubleTime();
	time = newtime - oldtime;

	if (!g_mainwindow.is_active && (cl.maxclients == 1))
	{
		// todo, skip updates if window isn't active
	}

	Host_Frame(time);

	oldtime = newtime;
}

/*	Version	*/

PLuint32 xenon::GetVersion()
{
	return ENGINE_VERSION_BUILD;
}

PLchar *xenon::GetVersionString()
{
	return va("%i", ENGINE_VERSION_BUILD);
}

PLint32 xenon::GetInterfaceVersion()
{
	return ENGINE_VERSION_INTERFACE;
}

/*	State	*/

PLbool xenon::IsRunning()
{
	return g_hostinitialized;
}

/*	Paths	*/

const PLchar *xenon::GetPath(XPath path)
{
	if ((path < XPATH_BASE) || (path >= XPATH_END))
	{
		Con_Warning("Invalid path selection! (%i)\n", path);
		return "";
	}

	switch (path)
	{
	case XPATH_BASE:return host_parms.basepath;
	case XPATH_FONTS:return g_state.path_fonts;
	case XPATH_LEVELS:return g_state.path_levels;
	case XPATH_MATERIALS:return g_state.path_materials;
	case XPATH_SCREENSHOTS:return g_state.path_screenshots;
	case XPATH_MODULES:return g_state.path_modules;
	case XPATH_SHADERS:return g_state.path_shaders;
	case XPATH_SOUNDS:return g_state.path_sounds;
	}
	// Unlikely to reach this point.
	return "";
}

/*	Legacy...	*/

XEngineImport g_launcher;
XEngineExport exports;

PL_MODULE_EXPORT XEngineExport *Engine_Main(XEngineImport *mImport)
{
	// Imports
	g_launcher.iVersion			= mImport->iVersion;
	g_launcher.PrintError		= mImport->PrintError;
	g_launcher.PrintMessage		= mImport->PrintMessage;
	g_launcher.PrintWarning		= mImport->PrintWarning;

	// Client
	exports.ClientDisconnect = CL_Disconnect;
	exports.GetClientTime = Client_GetTime;
	exports.CreateClientEntity = CL_NewTempEntity;
	exports.CreateDynamicLight = Client_AllocDlight;

	// Server
	exports.ServerShutdown = Host_ShutdownServer;

	// Material
	exports.LoadMaterial = Material_Load;
	exports.UnloadMaterial = Material_Clear;

	// Model
	exports.LoadModel = Mod_ForName;

	// Console
	exports.InsertConsoleCommand		= Cbuf_InsertText;
	exports.RegisterConsoleVariable		= Cvar_RegisterVariable;
	exports.SetConsoleVariable			= Cvar_Set;
	exports.ResetConsoleVariable		= Cvar_Reset;
	exports.GetConsoleVariableValue		= Cvar_VariableValue;
	exports.GetConsoleVariableBoolValue = ConsoleVariable_GetBoolValue;
	exports.Print						= Con_Printf;
	exports.PrintDev					= Con_DPrintf;

	return &exports;
}
