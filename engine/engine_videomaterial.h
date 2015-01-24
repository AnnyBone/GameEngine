#ifndef __ENGINEMATERIAL__
#define __ENGINEMATERIAL__

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

#define	MATERIAL_FLAG_PRESERVE	1	// Preserves the material during clear outs.
#define	MATERIAL_FLAG_ALPHA		2	// Declares that the given texture has an alpha channel.
#define	MATERIAL_FLAG_BLEND		4	// Ditto to the above, but tells us to use blending rather than alpha-test.
#define	MATERIAL_FLAG_ANIMATED	8	// This is a global flag; tells the material system to scroll through all skins.
#define	MATERIAL_FLAG_MIRROR	16	// Must be GLOBAL!
#define	MATERIAL_FLAG_UNUSED	32
#define	MATERIAL_FLAG_WATER		64	// Must be GLOBAL!

typedef struct
{
	MaterialProperty_t	iType;

	const char *ccName;
} MaterialType_t;

typedef struct
{
	struct gltexture_s	*gDiffuseTexture,		// Diffuse texture.
						*gDetailTexture,		// Detail map.
						*gFullbrightTexture,	// Fullbright map.
						*gSpecularTexture,		// Specular map.
						*gSphereTexture;		// Sphere map.

	float	fTextureScroll[2];	// If set, the texture will scroll by the given coordinates.

	unsigned int	iTextureWidth, iTextureHeight,	// Size of the skin.
					iFlags,							// Flags assigned for the current skin, affects how it's displayed/loaded.
					iType;							// Type of surface, e.g. wood, cement etc.
} MaterialSkin_t;

#define	MATERIAL_MAX		2048
#define	MATERIAL_MAX_SKINS	64

typedef struct
{
	int		iIdentification,	// Unique ID for the material.
			iFlags,				// Global material flags, flags that take priority over all additional skins.
			iSkins;				// Number of skins provided by this material.

	char	cPath[PLATFORM_MAX_PATH],	// Path of the material.
			cName[64];					// Name of the material.

	MaterialSkin_t	msSkin[MATERIAL_MAX_SKINS];

	bool	bBind;

	float	fAlpha;	// Alpha override.

	// Animation
	float	fAnimationSpeed;	// Speed to scroll through skins, if animation is enabled.
	double	dAnimationTime;		// Time until we animate again.
	int		iAnimationFrame;	// Current frame of the animation.
} Material_t;

void Material_Initialize(void);

Material_t *Material_Load(const char *ccPath);
Material_t *Material_Get(int iMaterialID);
Material_t *Material_GetByPath(const char *ccPath);
Material_t *Material_GetByName(const char *ccMaterialName);
Material_t *Material_GetDummy(void);

MaterialSkin_t *Material_GetSkin(Material_t *mMaterial, int iSkin);
MaterialSkin_t *Material_GetAnimatedSkin(Material_t *mMaterial);

#endif // __ENGINEMATERIAL__
