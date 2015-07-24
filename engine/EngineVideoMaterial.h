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

#ifndef __ENGINEMATERIAL__
#define __ENGINEMATERIAL__

#include "SharedMaterial.h"

// Global materials
extern Material_t	
	*mNoTexture,	// Used for materials that are missing.
	*mBlobShadow,	// Self explanitory.
	*mColour,
	*mConChars;		// Used for the console font.

// Do NOT change these.
#define	MATERIAL_COLOUR_WHITE	0
#define	MATERIAL_COLOUR_BLACK	1
#define	MATERIAL_COLOUR_BLUE	2

void Material_Initialize(void);
void Material_ClearAll(void);
void Material_Clear(Material_t *mMaterial);

bool Material_Precache(const char *ccPath);

Material_t *Material_Load(const char *ccPath);
Material_t *Material_Get(int iMaterialID);
Material_t *Material_GetByPath(const char *ccPath);
Material_t *Material_GetByName(const char *ccMaterialName);

MaterialSkin_t *Material_GetSkin(Material_t *mMaterial, int iSkin);
MaterialSkin_t *Material_GetAnimatedSkin(Material_t *mMaterial);

#endif // __ENGINEMATERIAL__
