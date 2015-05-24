/*	Copyright (C) 2011-2015 OldTimes Software
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
