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

#pragma once

#include "shared_engine.h"

typedef struct
{
	// Host Information
	char	host_username[PL_MAX_USERNAME];	// Current system username.

	char	
		path_materials[PLATFORM_MAX_PATH],
		path_textures[PLATFORM_MAX_PATH],
		path_levels[PLATFORM_MAX_PATH],
		path_screenshots[PLATFORM_MAX_PATH],
		path_modules[PLATFORM_MAX_PATH],
		path_fonts[PLATFORM_MAX_PATH],
		path_shaders[PLATFORM_MAX_PATH],
		path_sounds[PLATFORM_MAX_PATH];

	bool embedded;
} EngineGlobal_t;

plEXTERN_C_START

extern EngineGlobal_t	g_state;
extern EngineImport_t	g_launcher;

// System
double System_DoubleTime(void);

plEXTERN_C_END