/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
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

extern Material_t g_materials[MATERIAL_MAX];
extern int material_count;

void Material_Initialize(void);
void Material_ClearAll(void);
void Material_Clear(Material_t *material, bool force);
void Material_DrawObject(Material_t *material, vlDraw_t *object, bool ispost);
void Material_Draw(Material_t *material, vlVertex_t *ObjectVertex, VLPrimitive ObjectPrimitive, unsigned int ObjectSize, bool ispost);

bool Material_Precache(const char *path);

Material_t *Material_Load(const char *path);
Material_t *Material_Get(int id);
Material_t *Material_GetByPath(const char *path);
Material_t *Material_GetByName(const char *name);

void Material_SetSkin(Material_t *material, unsigned int skin);

MaterialSkin_t *Material_GetSkin(Material_t *material, unsigned int skin);
MaterialSkin_t *Material_GetAnimatedSkin(Material_t *material);

plEXTERN_C_END
