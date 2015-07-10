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

#ifndef __SHAREDENGINE__
#define __SHAREDENGINE__

#include "platform.h"

#include "SharedFlags.h"
#include "SharedMaterial.h"

//johnfitz -- stuff for 2d drawing control
typedef enum
{
	CANVAS_NONE,
	CANVAS_DEFAULT,
	CANVAS_CONSOLE,
	CANVAS_MENU,
	CANVAS_SBAR,
	CANVAS_WARPIMAGE,
	CANVAS_CROSSHAIR,
	CANVAS_BOTTOMLEFT,
	CANVAS_BOTTOMRIGHT,
	CANVAS_TOPRIGHT
} canvastype;

/*	Functions exported from the engine.
*/
typedef struct
{
	bool(*Initialize)(int argc, char *argv[], bool bEmbedded);	// Initializes the engine.
	bool(*IsRunning)(void);

	char*(*GetBasePath)(void);		// Gets the currently active game path.
	char*(*GetMaterialPath)(void);	// Gets the set material path.
	char*(*GetVersion)(void);		// Return the engine version.

	void(*Loop)(void);									// Main loop.
	void(*SetViewportSize)(int iWidth, int iHeight);
	void(*Shutdown)(void);								// Shutdown.

	Material_t*(*LoadMaterial)(const char *cPath);
	void(*UnloadMaterial)(Material_t *mMaterial);

	// Console
	void(*InsertConsoleCommand)(const char *cCommand);	// Sends the given command to the console.
	void(*Print)(char *cMessage, ...);					// Print a message to the console.
	void(*PrintDev)(char *cMessage, ...);				// Prints a developer message to the console.

	// Video
	void(*Video_PreFrame)(void);
	void(*Video_PostFrame)(void);

	void(*DrawConsole)(void);
	void(*DrawFPS)(void);
	void(*DrawString)(int x, int y, char *cMsg);
	void(*DrawLine)(MathVector3f_t mvStart, MathVector3f_t mvEnd);
	void(*DrawMaterialSurface)(Material_t *mMaterial, int iSkin, int x, int y, int w, int h, float fAlpha);
	void(*DrawModel)(void);		// Temporary!

	void(*ResetCanvas)(void);

	// Material Editor
	void(*MaterialEditorInitialize)(void);					// Initializes the material editor.
	void(*MaterialEditorDisplay)(Material_t *mMaterial);	// Displays a material in the editor.

	int	iVersion;
} EngineExport_t;

/*	Functions imported by the engine.
*/
typedef struct
{
	void(*PrintMessage)(char *text);
	void(*PrintWarning)(char *text);
	void(*PrintError)(char *text);

	int	iVersion;
} EngineImport_t;

#define ENGINE_VERSION	(sizeof(EngineImport_t)+sizeof(EngineExport_t))

#endif
