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

#ifndef ENGINE_MAIN_H
#define ENGINE_MAIN_H

#include "SharedModule.h"
#include "shared_engine.h"

typedef struct
{
	// Host Information
	char cLocalName[PLATFORM_MAX_USER];	// Current system username.

	char	
		cMaterialPath[PLATFORM_MAX_PATH],
		cTexturePath[PLATFORM_MAX_PATH],
		cLevelPath[PLATFORM_MAX_PATH],
		cScreenshotPath[PLATFORM_MAX_PATH],
		cModulePath[PLATFORM_MAX_PATH],
		cFontPath[PLATFORM_MAX_PATH],
		cShaderPath[PLATFORM_MAX_PATH],
		cSoundPath[PLATFORM_MAX_PATH];

	bool embedded;
} EngineGlobal_t;

#ifdef __cplusplus
extern "C" {
#endif

	extern EngineGlobal_t g_state;

	extern EngineImport_t g_launcher;

	// System
	double System_DoubleTime(void);

#ifdef __cplusplus
}
#endif

#endif