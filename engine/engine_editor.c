/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "engine_editor.h"

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

#include "video.h"
#include "engine_clientinput.h"
#include "engine_videoshadow.h"			// [14/10/2013] TODO: Obsolete! ~hogsy
#include "engine_console.h"
#include "engine_modmenu.h"

// Platform library
#include "platform_module.h"
#include "platform_filesystem.h"

cvar_t	cvEditorLightPreview	= {	"editor_lightpreview",	"0",	false,	false,	"Enables a preview showing the radius of each light."	};

pINSTANCE hToolInstance;

typedef enum
{
	EDITOR_MODE_CAMERA,	// Default 3D camera view.
	EDITOR_MODE_TOP,	// 2D top view mode.
	EDITOR_MODE_SIDE	// 2D side view mode.
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
	// TODO: Add a seperate command to handle this, rather than directly ~hogsy
	//Cmd_AddCommand("editor_loadlevel",Editor_LoadLevel);

	Cvar_RegisterVariable(&cvEditorLightPreview,NULL);

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

void Input_OpenTweakMenu(void);

/*  Command function to allow us to launch the editor.
*/
void Editor_Launch(void)
{
	// Don't allow us to launch again.
	if(Editor.bEnabled)
		return;

	Con_Printf("\nLaunching editor...\n");

	key_dest = KEY_EDITOR;

	{
		Con_Printf(" Loading textures: \n");

		pFileSystem_ScanDirectory(va("./%s/textures/",com_gamedir),Editor_LoadTexture);
	}

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

	switch(Editor.iDrawMode)
	{
	case EDITOR_DRAW_WIREFRAME:
		break;
	case EDITOR_DRAW_TEXTURED:
		break;
	}

    GL_SetCanvas(CANVAS_DEFAULT);

	Draw_Fill(0,0,Video.iWidth,20,0,0,0,1.0f);
	R_DrawString(10,10,va("Camera: origin(%i %i %i), angles(%i %i %i)",
		(int)r_refdef.vieworg[pX],
		(int)r_refdef.vieworg[pY],
		(int)r_refdef.vieworg[pZ],
		(int)r_refdef.viewangles[pX],
		(int)r_refdef.viewangles[pY],
		(int)r_refdef.viewangles[pZ]));

	// [1/12/2013] TODO: Draw cursor... ~hogsy
}

void Editor_Shutdown(void)
{
	if(!Editor.bEnabled)
		return;

	// [1/12/2013] TODO: Unload textures... ~hogsy

	Editor.bEnabled = false;
}
