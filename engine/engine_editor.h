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

#ifndef __ENGINEEDITOR__
#define	__ENGINEEDITOR__

#include "shared_module.h"

extern	cvar_t	cvEditorLightPreview;

typedef struct
{
	bool bEnabled;       // Is the editor active?
	bool bToolsLoaded;   // Tools loaded (KBSP etc.)

	int		iDrawMode,      // Active draw mode.
			iSelectMode;    // Active selection mode.

    char *cMap;	// Currently loaded level.
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