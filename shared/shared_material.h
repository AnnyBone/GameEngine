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

#define	MATERIAL_FLAG_ALPHA			(1 << 1)	// Declares that the given texture has an alpha channel.
#define	MATERIAL_FLAG_BLEND			(1 << 2)	// Ditto to the above, but tells us to use blending rather than alpha-test.
#define	MATERIAL_FLAG_ANIMATED		(1 << 3)	// This is a global flag; tells the material system to scroll through all skins.
#define	MATERIAL_FLAG_MIRROR		(1 << 4)	// Must be GLOBAL!
#define	MATERIAL_FLAG_NEAREST		(1 << 5)	// Forces the texture to be loaded with nearest filtering.
#define	MATERIAL_FLAG_WATER			(1 << 6)	// Must be GLOBAL!
#define	MATERIAL_FLAG_ADDITIVE		(1 << 7)	// Renders the current skin as an additive.
#define	MATERIAL_FLAG_ALPHATRICK	(1 << 8)	// Skips/enables the alpha trick (handy for UI elements!)
#define	MATERIAL_FLAG_PRESERVE		(1 << 9)	// Preserves the material during clear outs.

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
	MATERIAL_TEXTURE_NORMAL,		// Defines the direction of a pixel's normal.
	MATERIAL_TEXTURE_DETAIL,		// Detail map is blended with other layers to make textures appear more detailed.
	MATERIAL_TEXTURE_LIGHTMAP,

	MATERIAL_TEXTURE_MAX
} MaterialTextureType_t;

typedef struct
{
	gltexture_t	*gMap;

	bool	matrixmod;	// Modify texture matrix?

	plVector2f_t	scroll;

	float	fRotate, scale;

	unsigned int uiWidth, uiHeight;	// Width and height of the texture.
	unsigned int uiFlags;			// Size of the texture.

	MaterialTextureType_t mttType;	// Sphere, fullbright, or what have you.

	vlTextureEnvironmentMode_t env_mode;
} MaterialTexture_t;

#include "./shared_client_shader.h"

typedef struct MaterialSkin_s
{
	// todo, this shouldn't be hardcoded size!!!!!
	MaterialTexture_t texture[16];

	CoreShaderProgram *program;	// Current shader.

	unsigned int
		uiFlags,		// Flags assigned for the current skin, affects how it's displayed/loaded.
		num_textures,	// Num of textures assigned within the skin.
		uiType;			// Type of surface, e.g. wood, cement etc.
} MaterialSkin_t;

#define	MATERIAL_MAX		2048	// Should ALWAYS be below the maximum texture allowance.
#define	MATERIAL_MAX_SKINS	128		// These also count as frames for animation.

typedef struct Material_s
{
	int
		id,					// Unique ID for the material.
		flags;				// Global material flags, flags that take priority over all additional skins.			

	char	
		cPath[PLATFORM_MAX_PATH],	// Path of the material.
		cName[64];					// Name of the material.

	// Skins
	MaterialSkin_t	skin[MATERIAL_MAX_SKINS];
	unsigned int	num_skins;							// Number of skins provided by this material.
	unsigned int	current_skin;

	bool bind;	// If true, texture will be bound.

	// Overrides
	bool
		override_lightmap,
		override_wireframe;	// Override tris for this material.

	float fAlpha;	// Alpha override.

	// Animation
	float			animation_speed;	// Speed to scroll through skins, if animation is enabled.
	unsigned int	animation_frame;	// Current frame for animation.
	double			animation_time;		// Time until we animate again.
} Material_t;