/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __ENGINEEDITOR__
#define	__ENGINEEDITOR__

#include "quakedef.h"

#include "shared_module.h"

extern	cvar_t	cvEditorLightPreview;

typedef struct
{
	bool	bEnabled,       // Is the editor active?
            bToolsLoaded;   // Tools loaded (KBSP etc.)

	int		iDrawMode,      // Active draw mode.
			iSelectMode;    // Active selection mode.

    char    *cMap;          // Currently loaded level.
} Editor_t;

Editor_t	Editor;

void Editor_Draw(void);
void Editor_Initialize(void);
void Editor_Input(int iKey);

/*
	Material Editor
*/

extern bool bMaterialEditorInitialized;

void MaterialEditor_Initialize(void);
void MaterialEditor_Input(int iKey);
void MaterialEditor_Draw(void);
void MaterialEditor_Display(Material_t *mDisplayMaterial);

#endif