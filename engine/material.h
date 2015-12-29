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

#ifndef ENGINE_MATERIAL_H
#define ENGINE_MATERIAL_H

#include "shared_material.h"

#ifdef __cplusplus
extern "C" {
#endif

	// Global materials
	extern Material_t
		*g_mHDAccess,
		*g_mMissingMaterial,	// Used for materials that are missing.
		*g_mBlobShadow,	// Self explanitory.
		*g_mGlobalColour,
		*g_mGlobalConChars;		// Used for the console font.

	void Material_Initialize(void);
	void Material_ClearAll(void);
	void Material_Clear(Material_t *mMaterial);
	void Material_Draw(Material_t *Material, int Skin, VideoObjectVertex_t *ObjectVertex, VideoPrimitive_t ObjectPrimitive, unsigned int ObjectSize, bool bPost);

	bool Material_Precache(const char *ccPath);

	Material_t *Material_Load(const char *ccPath);
	Material_t *Material_Get(int iMaterialID);
	Material_t *Material_GetByPath(const char *ccPath);
	Material_t *Material_GetByName(const char *ccMaterialName);

	MaterialSkin_t *Material_GetSkin(Material_t *mMaterial, int iSkin);
	MaterialSkin_t *Material_GetAnimatedSkin(Material_t *mMaterial);

#ifdef __cplusplus
}
#endif

#endif // ENGINE_MATERIAL_H
