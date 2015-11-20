/*	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef SHARED_MATERIAL_H
#define	SHARED_MATERIAL_H

#include "shared_texture.h"

typedef enum
{
	MATERIAL_TYPE_NONE,		// No assigned property, used as default.
	MATERIAL_TYPE_METAL,	// Metal property.
	MATERIAL_TYPE_GLASS,	// Glass property.
	MATERIAL_TYPE_CONCRETE,	// Concrete property.
	MATERIAL_TYPE_GRASS,	// Grass property.
	MATERIAL_TYPE_WOOD,		// Wood property.
	MATERIAL_TYPE_WATER,	// Water property.
	MATERIAL_TYPE_DIRT,		// Dirt property.
	MATERIAL_TYPE_MUD,		// Mud property.
	MATERIAL_TYPE_RUBBER,	// Rubber property.
	MATERIAL_TYPE_FLESH,	// Flesh property.
	MATERIAL_TYPE_CARPET,	// Carpet property.
	MATERIAL_TYPE_SNOW,		// Snow property.

	MATERIAL_TYPE_MAX		// This isn't valid, don't use it (just used for utility).
} MaterialProperty_t;

// Do NOT change these.
#define	MATERIAL_COLOUR_WHITE	0
#define	MATERIAL_COLOUR_BLACK	1
#define	MATERIAL_COLOUR_BLUE	2

#define	MATERIAL_FLAG_PRESERVE	1	// Preserves the material during clear outs.
#define	MATERIAL_FLAG_ALPHA		2	// Declares that the given texture has an alpha channel.
#define	MATERIAL_FLAG_BLEND		4	// Ditto to the above, but tells us to use blending rather than alpha-test.
#define	MATERIAL_FLAG_ANIMATED	8	// This is a global flag; tells the material system to scroll through all skins.
#define	MATERIAL_FLAG_MIRROR	16	// Must be GLOBAL!
#define	MATERIAL_FLAG_NEAREST	32	// Forces the texture to be loaded with nearest filtering.
#define	MATERIAL_FLAG_WATER		64	// Must be GLOBAL!

typedef struct
{
	MaterialProperty_t	iType;

	const char *ccName;
} MaterialType_t;

typedef enum
{
	MATERIAL_TEXTURE_DIFFUSE,		// Basic diffuse layer.
	MATERIAL_TEXTURE_SPHERE,		// Spherical mapping.
	MATERIAL_TEXTURE_FULLBRIGHT,	// Adds highlights to the texture.
	MATERIAL_TEXTURE_DETAIL,		// Detail map is blended with other layers to make textures appear more detailed.

	MATERIAL_TEXTURE_MAX
} MaterialTextureType_t;

typedef struct
{
	gltexture_t	*gMap;

	bool	bManipulated;

	MathVector2f_t	vScroll;

	float	fRotate;

	unsigned int uiWidth, uiHeight;	// Width and height of the texture.
	unsigned int uiFlags;			// Size of the texture.

	MaterialTextureType_t mttType;	// Sphere, fullbright, or what have you.

	VideoTextureEnvironmentMode_t EnvironmentMode;
} MaterialTexture_t;

typedef struct
{
	const char *ccName;
} MaterialShader_t;

typedef struct
{
	MaterialTexture_t mtTexture[VIDEO_MAX_UNITS];

	unsigned int
		uiFlags,	// Flags assigned for the current skin, affects how it's displayed/loaded.
		uiTextures,	// Num of textures assigned within the skin.
		uiType;		// Type of surface, e.g. wood, cement etc.
} MaterialSkin_t;

#define	MATERIAL_MAX		2048	// Should ALWAYS be below the maximum texture allowance.
#define	MATERIAL_MAX_SKINS	128		// These also count as frames for animation.

typedef struct Material_s
{
	int		
		iIdentification,	// Unique ID for the material.
		iFlags,				// Global material flags, flags that take priority over all additional skins.
		iSkins;				// Number of skins provided by this material.

	char	
		cPath[PLATFORM_MAX_PATH],	// Path of the material.
		cName[64];					// Name of the material.

	MaterialSkin_t msSkin[MATERIAL_MAX_SKINS];
	MaterialShader_t msShader;

	bool 
		bBind,
		bWireframeOverride;	// Override tris for this material.

	float fAlpha;	// Alpha override.

	// Animation
	float	fAnimationSpeed;	// Speed to scroll through skins, if animation is enabled.
	double	dAnimationTime;		// Time until we animate again.
	int		iAnimationFrame;	// Current frame of the animation.
} Material_t;

#endif // !SHARED_MATERIAL_H