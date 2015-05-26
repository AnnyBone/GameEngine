/*	Copyright (C) 2011-2015 OldTimes Software
*/

#ifndef __SHAREDENGINE__
#define __SHAREDENGINE__

#include "../platform/include/platform.h"

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

	// Video
	void(*Video_PreFrame)(void);
	void(*Video_PostFrame)(void);
	void(*DrawConsole)(void);
	void(*DrawFPS)(void);
	void(*DrawString)(int x, int y, char *cMsg);
	void(*DrawLine)(MathVector3f_t mvStart, MathVector3f_t mvEnd);
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
