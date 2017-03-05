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

PL_EXTERN_C

// Global materials
extern Material
	*g_mHDAccess,
	*g_mMissingMaterial,	// Used for materials that are missing.
	*g_mBlobShadow,			// Self explanitory.
	*g_mGlobalColour,
	*g_mGlobalConChars;		// Used for the console font.

extern Material g_materials[MATERIAL_MAX];
extern int material_count;

void Material_Initialize(void);
void Material_ClearAll(void);
void Material_Clear(Material *material, bool force);
void Material_DrawObject(Material *material, PLMesh *object, bool ispost);
void Material_Draw(Material *material, PLVertex *ObjectVertex, PLPrimitive ObjectPrimitive, unsigned int ObjectSize, bool ispost);

bool Material_Precache(const char *path);

Material *Material_Load(const char *path);
Material *Material_Get(int id);
Material *Material_GetByPath(const char *path);
Material *Material_GetByName(const char *name);

void Material_SetSkin(Material *material, unsigned int skin);

MaterialSkin *Material_GetSkin(Material *material, unsigned int skin);
MaterialSkin *Material_GetAnimatedSkin(Material *material);

PL_EXTERN_C_END
