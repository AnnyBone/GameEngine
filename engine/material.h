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

#include "shared_material.h"

plEXTERN_C_START

// Global materials
extern Material_t
	*g_mHDAccess,
	*g_mMissingMaterial,	// Used for materials that are missing.
	*g_mBlobShadow,			// Self explanitory.
	*g_mGlobalColour,
	*g_mGlobalConChars;		// Used for the console font.

void Material_Initialize(void);
void Material_ClearAll(void);
void Material_Clear(Material_t *material);
void Material_DrawObject(Material_t *material, VideoObject_t *object, bool ispost);
void Material_Draw(Material_t *material, VideoVertex_t *ObjectVertex, VideoPrimitive_t ObjectPrimitive, unsigned int ObjectSize, bool ispost);

bool Material_Precache(const char *path);

Material_t *Material_Load(const char *path);
Material_t *Material_Get(int id);
Material_t *Material_GetByPath(const char *path);
Material_t *Material_GetByName(const char *name);

void Material_SetSkin(Material_t *material, unsigned int skin);

MaterialSkin_t *Material_GetSkin(Material_t *material, unsigned int skin);
MaterialSkin_t *Material_GetAnimatedSkin(Material_t *material);

plEXTERN_C_END
