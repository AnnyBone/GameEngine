/*	Copyright (C) 2011-2014 OldTimes Software
*/
#include "KatEditor.h"

/*
	The Katana Editor is perhaps the biggest addition to
	the engine. If this is finished in-time for OpenKatana's
	release then big woop! This idea was inspired by the
	1998 version of Prey's editor (also known as Preditor)
	which was a unique editor similar to the Unreal Engine's
	editor in that it shows your map while you work and the
	editor environment is in-engine which means that what you
	see is what you'll get (WYSIWYG).
	Preditor supported multiple ways of working, we'll be
	going for Matt's favourite approach and allowing users
	to edit the map	from a single window with a realtime preview
	of the map in view...
*/

#include "engine_video.h"
#include "engine_input.h"
#include "KatGL.h"			// [14/10/2013] TODO: Obsolete! ~hogsy
#include "engine_console.h"

#include "../platform/include/platform_module.h"
#include "../platform/include/platform_filesystem.h"

gINSTANCE hToolInstance;

ModuleEditor_t	*mToolModule;

typedef enum
{
	EDITOR_MODE_CAMERA,	// Default 3D camera view.
	EDITOR_MODE_TOP		// Secondary top view mode.
} EditorMode_t;

// Draw Modes
typedef enum
{
	EDITOR_DRAW_FLAT,       // Draw everything as flat-shaded.
	EDITOR_DRAW_WIREFRAME,  // Draw everything as a wireframe.
	EDITOR_DRAW_TEXTURED    // Draw everything normally: textured and shaded.
} EditorDraw_t;

// Selection Modes
typedef enum
{
	EDITOR_SELECT_VERTEX,
	EDITOR_SELECT_EDGE
} EditorSelect_t;

void Editor_Launch(void);

/*  Initialization function.
*/
void Editor_Initialize(void)
{
	Editor.bEnabled = false;

	Cmd_AddCommand("editor",Editor_Launch);

	if(COM_CheckParm("-editor"))
		Editor_Launch();
}

/*  Load a texture, which will be later used for displaying in our level.
*/
void Editor_LoadTexture(const char *ccTexture)
{
	Con_Printf("   %s\n",ccTexture);
}

/*	Opens up a level for editing.
*/
void Editor_LoadLevel(const char *ccLevel)
{
	if(strstr(ccLevel,".map"))
	{
	}
	// [6/2/2014] ".level" is our new extension ~hogsy
	else if(strstr(ccLevel,".level"))
	{
	}
	else
		Con_Warning("Unknown level format! (%s)\n",ccLevel);
}

/*  Command function to allow us to launch the editor.
*/
void Editor_Launch(void)
{
	// [27/11/2013] Don't let us launch again ~hogsy
	if(Editor.bEnabled)
		return;

	Con_Printf("\nLaunching editor...\n");

	key_dest = KEY_EDITOR;

	{
		Con_Printf(" Loading textures: \n");

		gFileSystem_ScanDirectory(va("./%s/textures/",com_gamedir),Editor_LoadTexture);
	}

#if 0
	{
		ModuleImport_t	Import;

		if(mToolModule)
			GIPLm_Unload(hToolInstance);

		Import.Con_DPrintf				= Con_DPrintf;
		Import.Con_Printf				= Con_Printf;
		Import.Con_Warning				= Con_Warning;
		Import.Cvar_RegisterVariable	= Cvar_RegisterVariable;

		mToolModule = (ModuleEditor_t*)gModule_Load(hToolInstance,MODULE_EDITOR,"Toolkit_Main",&Import);
		if(!mToolModule)
		{
			Console_ErrorMessage(false,MODULE_EDITOR,"Failed to find "MODULE_EDITOR".");

			GIPLm_Unload(hToolInstance);
			return;
		}
		else if(mToolModule->iVersion != MODULE_VERSION)
		{
			Console_ErrorMessage(false,MODULE_EDITOR,va("Size mismatch (recieved %i, expected %i).",mToolModule->iVersion,MODULE_VERSION));

			GIPLm_Unload(hToolInstance);
			return;
		}

		Con_Printf(" Tools loaded!\n")
	}
#endif

	Input_ActivateMouse();

	Editor.bEnabled = true;
}

/*  Handle input.
*/
void Editor_Input(int iKey)
{
	switch(iKey)
	{
	case K_UPARROW:
	case K_DOWNARROW:
	case K_LEFTARROW:
	case K_RIGHTARROW:
		break;
	}
}

void Editor_Draw(void)
{
	if(!Editor.bEnabled)
		return;

#if 0
	switch(Editor.iDrawMode)
	{
	case EDITOR_DRAW_WIREFRAME:
	case EDITOR_DRAW_TEXTURED:
		glEnable(GL_FRONT);
		glEnable(GL_CULL_FACE);
		glShadeModel(GL_FLAT);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
#endif

	Draw_Fill(0,0,Video.iWidth,20,0,0,0,1.0f);
	R_DrawString(10,10,va("Camera: origin(%i %i %i), angles(%i %i %i)",
		(int)r_refdef.vieworg[X],
		(int)r_refdef.vieworg[Y],
		(int)r_refdef.vieworg[Z],
		(int)r_refdef.viewangles[X],
		(int)r_refdef.viewangles[Y],
		(int)r_refdef.viewangles[Z]));

	// [1/12/2013] TODO: Draw cursor... ~hogsy
}

void Editor_Shutdown(void)
{
	if(!Editor.bEnabled)
		return;

	// [1/12/2013] TODO: Unload textures... ~hogsy

	Editor.bEnabled = false;
}