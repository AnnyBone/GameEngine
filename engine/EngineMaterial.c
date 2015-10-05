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

#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoShader.h"
#include "EngineScript.h"

/*
	Material System

	This is pretty much written from scratch without using anything else for reference,
	more just for inspiration. This has been a pretty weird learning experience for me
	since I've never written anything out like this before but it works and does the job
	it was designed to do.
	~hogsy
*/

bool bInitialized = false;

Material_t	mMaterials[MATERIAL_MAX];	// Global array.

MaterialType_t	MaterialTypes[]=
{
	{ MATERIAL_TYPE_NONE, "default" },
	{ MATERIAL_TYPE_METAL, "metal" },
	{ MATERIAL_TYPE_GLASS, "glass" },
	{ MATERIAL_TYPE_CONCRETE, "concrete" },
	{ MATERIAL_TYPE_WOOD, "wood" },
	{ MATERIAL_TYPE_DIRT, "dirt" },
	{ MATERIAL_TYPE_RUBBER, "rubber" },
	{ MATERIAL_TYPE_WATER, "water" },
	{ MATERIAL_TYPE_FLESH, "flesh" },
	{ MATERIAL_TYPE_SNOW, "snow" },
	{ MATERIAL_TYPE_MUD, "mud" }
};

int	iMaterialCount = -1;

Material_t *Material_Allocate(void);

// Global Materials
Material_t *g_mHDAccess;
Material_t *g_mMissingMaterial;
Material_t *g_mBlobShadow;
Material_t *g_mGlobalColour;
Material_t *g_mGlobalConChars;

void Material_List(void);

void Material_Initialize(void)
{
	if(bInitialized)
		return;

	Con_Printf("Initializing material system...\n");

	Cmd_AddCommand("material_list", Material_List);

	// Must be set to initialized before anything else.
	bInitialized = true;

	// Load base materials...

	g_mMissingMaterial = Material_Load("base/missing");
	if (!g_mMissingMaterial)
		Sys_Error("Failed to load notexture material!\n");

	g_mGlobalConChars = Material_Load("engine/conchars");
	if (!g_mGlobalConChars)
		Sys_Error("Failed to load conchars material!\n");

	g_mGlobalColour = Material_Load("colours/colours");
	if (!g_mGlobalColour)
		Sys_Error("Failed to load colours material!\n");

	g_mHDAccess = Material_Load("sprites/base/access");
	g_mBlobShadow = Material_Load("engine/shadow");
}

/*	Lists all the currently active materials.
*/
void Material_List(void)
{
	int i;

	Con_Printf("Listing materials...\n");

	for (i = 0; i < iMaterialCount; i++)
	{
		if (!mMaterials[i].cName[0] && !mMaterials[i].cPath[0])
			continue;

		Con_Printf(" %s (%s) (%i)\n", mMaterials[i].cName, mMaterials[i].cPath, mMaterials[i].iSkins);
	}

	Con_Printf("\nListed %i active materials!\n", i);
}

/*
	Management
*/

Material_t *Material_Allocate(void)
{
	iMaterialCount++;
	if (iMaterialCount > MATERIAL_MAX)
		Sys_Error("Failed to add new material onto global array! (%i)\n", iMaterialCount);

#ifdef _MSC_VER
#pragma warning(suppress: 6386)
#endif
	mMaterials[iMaterialCount].cName[0] = 0;
	mMaterials[iMaterialCount].iIdentification = iMaterialCount;
	mMaterials[iMaterialCount].fAlpha = 1.0f;
	mMaterials[iMaterialCount].bBind = true;

	return &mMaterials[iMaterialCount];
}

/*	Clears out the specific skin.
*/
void Material_ClearSkin(Material_t *mMaterial, int iSkin)
{
	unsigned int i;
	MaterialSkin_t *mSkin;

	mSkin = Material_GetSkin(mMaterial, iSkin);
	if (!mSkin)
		Sys_Error("Attempted to clear invalid skin! (%s) (%i)\n", mMaterial->cPath, iSkin);

#ifdef _MSC_VER
#pragma warning(suppress: 6011)
#endif
	for (i = 0; i < mSkin->uiTextures; i++)
		TexMgr_FreeTexture(mSkin->mtTexture[i].gMap);
}

void Material_Clear(Material_t *mMaterial)
{
	int i;

	if (!(mMaterial->iFlags & MATERIAL_FLAG_PRESERVE))
	{
		for (i = 0; i < mMaterial->iSkins; i++)
			Material_ClearSkin(mMaterial, i);

		memset(mMaterial, 0, sizeof(Material_t));

		iMaterialCount--;
	}
}

/*	Clears all the currently active materials.
*/
void Material_ClearAll(void)
{
	int	i;

	for (i = iMaterialCount; i > 0; i--)
		Material_Clear(&mMaterials[i]);

	// TODO: Reshuffle and move preserved to start.
}

MaterialSkin_t *Material_GetSkin(Material_t *mMaterial,int iSkin)
{
	if (iSkin < 0 || iSkin > MATERIAL_MAX_SKINS)
		Sys_Error("Invalid skin identification, should be greater than 0 and less than %i! (%i)\n", MATERIAL_MAX_SKINS, iSkin);
	else if (iSkin > mMaterial->iSkins)
		Sys_Error("Attempted to get an invalid skin! (%i) (%s)\n", iSkin, mMaterial->cName);

	return &mMaterial->msSkin[iSkin];
}

/*	Get an animated skin.
*/
MaterialSkin_t *Material_GetAnimatedSkin(Material_t *mMaterial)
{
	if (mMaterial->dAnimationTime < cl.time)
	{
		// Increment current frame.
		mMaterial->iAnimationFrame++;

		// If we're beyond the frame count, step back to 0.
		if (mMaterial->iAnimationFrame >= mMaterial->iSkins)
			mMaterial->iAnimationFrame = 0;

		mMaterial->dAnimationTime = cl.time + ((double)mMaterial->fAnimationSpeed);
	}

	return Material_GetSkin(mMaterial,mMaterial->iAnimationFrame);
}

/*	Returns a material from the given ID.
*/
Material_t *Material_Get(int iMaterialID)
{
	int i;

	// The identification would never be less than 0, and never more than our maximum.
	if (iMaterialID < 0 || iMaterialID > MATERIAL_MAX)
	{
		Con_Warning("Invalid material ID! (%i)\n",iMaterialID);
		return NULL;
	}

	for (i = 0; i < iMaterialCount; i++)
		if (mMaterials[i].iIdentification == iMaterialID)
		{
			mMaterials[i].bBind = true;

			return &mMaterials[i];
		}

	return NULL;
}

/*	Returns true on success.
	Unfinished
*/
Material_t *Material_GetByName(const char *ccMaterialName)
{
	int i;

	if(ccMaterialName[0] == ' ')
	{
		Con_Warning("Attempted to find material, but recieved invalid material name!\n");
		return NULL;
	}

	for (i = 0; i < iMaterialCount; i++)
		// If the material has no name, then it's not valid.
		if (mMaterials[i].cName[0])
			if (!strncmp(mMaterials[i].cName, ccMaterialName, sizeof(mMaterials[i].cName)))
				return &mMaterials[i];

	return NULL;
}

Material_t *Material_GetByPath(const char *ccPath)
{
	int i;

	if(ccPath[0] == ' ')
	{
		Con_Warning("Attempted to find material, but recieved invalid path!\n");
		return NULL;
	}
	
	for (i = 0; i < iMaterialCount; i++)
		if (mMaterials[i].cPath[0])
			if (!strncmp(mMaterials[i].cPath, ccPath, sizeof(mMaterials[i].cPath)))
				return &mMaterials[i];

	return NULL;
}

gltexture_t *Material_LoadTexture(Material_t *mMaterial, MaterialSkin_t *mCurrentSkin, char *cArg)
{
	int	iTextureFlags = TEXPREF_ALPHA|TEXPREF_MIPMAP;
	uint8_t *bTextureMap;

	// Check if it's trying to use a built-in texture.
	if (cArg[0] == '@')
	{
		cArg++;

		if (!Q_strcasecmp(cArg, "notexture"))
			return notexture;
		else
		{
			Con_Warning("Attempted to set invalid internal texture! (%s)\n", mMaterial->cPath);
			return notexture;
		}
	}

	// Ensure we haven't loaded the texture in already...
	gltexture_t *gTexture = TexMgr_GetTexture(cArg);
	if (gTexture)
#if 0	// Debugging
	{
		Con_Printf("Found already existing sample! (%s) (%s)", gTexture->name, mMaterial->cPath);
		return gTexture;
	}
#else
		return gTexture;
#endif

	bTextureMap = Image_LoadImage(cArg,
		&mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiWidth,
		&mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiHeight);
	if (bTextureMap)
	{
		// Warn about incorrect sizes.
		if ((mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiWidth % 2) || (mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiHeight % 2))
		{

			Con_Warning("Texture size is not multiple of 2! (%s) (%ix%i)\n", cArg,
				mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiWidth,
				mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiHeight);

#if 1
			// Pad the image.
			iTextureFlags |= TEXPREF_PAD;
#endif
		}

		if (mMaterial->iFlags & MATERIAL_FLAG_PRESERVE)
			iTextureFlags |= TEXPREF_PERSIST;

		if (mCurrentSkin->uiFlags & MATERIAL_FLAG_NEAREST)
			iTextureFlags |= TEXPREF_NEAREST;

		return TexMgr_LoadImage(NULL, cArg,
			mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiWidth,
			mCurrentSkin->mtTexture[mCurrentSkin->uiTextures].uiHeight,
			SRC_RGBA, bTextureMap, cArg, 0, iTextureFlags);
	}

	Con_Warning("Failed to load texture! (%s) (%s)\n", cArg, mMaterial->cPath);

	return notexture;
}

/*
	Scripting
*/

typedef enum
{
	MATERIAL_FUNCTION_NONE,

	MATERIAL_FUNCTION_MATERIAL,
	MATERIAL_FUNCTION_SKIN,
	MATERIAL_FUNCTION_TEXTURE,
	MATERIAL_FUNCTION_UNIVERSAL
} MaterialFunctionType_t;

MaterialFunctionType_t	mftMaterialState;	// Indicates that any settings applied are global.

void Material_CheckFunctions(Material_t *mNewMaterial);

// Material Functions...

typedef struct
{
	const char	*ccName;

	MaterialTextureType_t	mttType;
} MaterialTextureTypeX_t;

MaterialTextureTypeX_t mttMaterialTypes[] =
{
	{ "diffuse", MATERIAL_TEXTURE_DIFFUSE },		// Default
	{ "detail", MATERIAL_TEXTURE_DETAIL },			// Detail map
	{ "sphere", MATERIAL_TEXTURE_SPHERE },			// Sphere map
	{ "fullbright", MATERIAL_TEXTURE_FULLBRIGHT }	// Fullbright map
};

void _Material_SetTextureType(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg);

void _Material_SetType(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	switch (mftContext)
	{
	case MATERIAL_FUNCTION_SKIN:
	{
		int	iMaterialType = Q_atoi(cArg);

		// Ensure that the given type is valid.
		if ((iMaterialType < MATERIAL_TYPE_NONE) || (iMaterialType >= MATERIAL_TYPE_MAX))
			Con_Warning("Invalid material type! (%i)\n", iMaterialType);

		mCurrentMaterial->msSkin[mCurrentMaterial->iSkins].uiType = iMaterialType;
	}
	break;
	case MATERIAL_FUNCTION_TEXTURE:
		_Material_SetTextureType(mCurrentMaterial, mftContext, cArg);
		break;
	default:
		Sys_Error("Invalid context!\n");
	}
}

void _Material_SetWireframe(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	mCurrentMaterial->bWireframeOverride = (bool)atoi(cArg);
}

void _Material_SetShader(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	mCurrentMaterial->msShader.ccName = cArg;
}

void _Material_SetAlpha(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	mCurrentMaterial->fAlpha = strtof(cArg, NULL);
}

void _Material_SetAnimationSpeed(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	mCurrentMaterial->fAnimationSpeed = strtof(cArg, NULL);
}

// Skin Functions...

void _Material_AddSkin(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	// Proceed to the next line.
	Script_GetToken(true);

	if (cToken[0] == '{')
	{
		while (true)
		{
			if (!Script_GetToken(true))
			{
				Con_Warning("End of field without closing brace! (%s) (%i)\n", mCurrentMaterial->cPath, iScriptLine);
				break;
			}

			mftMaterialState = MATERIAL_FUNCTION_SKIN;

			if (cToken[0] == '}')
			{
				mCurrentMaterial->iSkins++;
				break;
			}
			// '$' declares that the following is a function.
			else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
				Material_CheckFunctions(mCurrentMaterial);
			// '%' declares that the following is a variable.
			else if (cToken[0] == SCRIPT_SYMBOL_VARIABLE)
			{
				/*	TODO:
				* Collect variable
				* Check it against internal solutions
				* Otherwise declare it, figure out where/how it's used
				*/
			}
			else
			{
				Con_Warning("Invalid field! (%s) (%i)\n", mCurrentMaterial->cPath, iScriptLine);
				break;
			}
		}
	}
	else
		Con_Warning("Invalid skin, no opening brace! (%s) (%i)\n", mCurrentMaterial->cPath, iScriptLine);
}

// Texture Functions...

void _Material_AddTexture(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	char cTexturePath[MAX_QPATH];

	MaterialSkin_t *msSkin = Material_GetSkin(mCurrentMaterial, mCurrentMaterial->iSkins);
	if (!msSkin)
		Sys_Error("Failed to get skin!\n");

#ifdef _MSC_VER
#pragma warning(suppress: 6011)
#endif
	msSkin->mtTexture[msSkin->uiTextures].EnvironmentMode = VIDEO_TEXTURE_MODE_MODULATE;
	msSkin->mtTexture[msSkin->uiTextures].bManipulated = false;
	msSkin->mtTexture[msSkin->uiTextures].fRotate = 0;
	msSkin->mtTexture[msSkin->uiTextures].mttType = MATERIAL_TEXTURE_DIFFUSE;
	msSkin->mtTexture[msSkin->uiTextures].vScroll[0] = 0;
	msSkin->mtTexture[msSkin->uiTextures].vScroll[1] = 0;

	Q_strcpy(cTexturePath, cArg);

	// Get following line.
	Script_GetToken(true);

	if (cToken[0] == '{')
	{
		for (;;)
		{
			if (!Script_GetToken(true))
			{
				Con_Warning("End of field without closing brace! (%s) (%i)\n", mCurrentMaterial->cPath, iScriptLine);
				break;
			}

			// Update state.
			mftMaterialState = MATERIAL_FUNCTION_TEXTURE;

			if (cToken[0] == '}')
			{
				msSkin->mtTexture[msSkin->uiTextures].gMap = Material_LoadTexture(mCurrentMaterial, msSkin, cTexturePath);
				msSkin->uiTextures++;
				break;
			}
			// '$' declares that the following is a function.
			else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
				Material_CheckFunctions(mCurrentMaterial);
			// '%' declares that the following is a variable.
			else if (cToken[0] == SCRIPT_SYMBOL_VARIABLE)
			{
				/*	TODO:
				* Collect variable
				* Check it against internal solutions
				* Otherwise declare it, figure out where/how it's used
				*/
			}
			else
			{
				Con_Warning("Invalid field! (%s) (%i)\n", mCurrentMaterial->cPath, iScriptLine);
				break;
			}
		}
	}
	else
#if 1
		Con_Warning("Invalid skin, no opening brace! (%s) (%i)\n", mCurrentMaterial->cPath, iScriptLine);
#else
	{
		msSkin->mtTexture[msSkin->uiTextures].gMap = Material_LoadTexture(mCurrentMaterial, msSkin, cTexturePath);
		msSkin->uiTextures++;

		Script_GetToken(true);
	}
#endif
}

void _Material_SetTextureType(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	MaterialSkin_t	*msSkin;
	int				i;

	msSkin = Material_GetSkin(mCurrentMaterial, mCurrentMaterial->iSkins);

	// Search through and copy each flag into the materials list of flags.
	for (i = 0; i < pARRAYELEMENTS(mttMaterialTypes); i++)
		if (strstr(cArg, mttMaterialTypes[i].ccName))
			msSkin[mCurrentMaterial->iSkins].mtTexture[mCurrentMaterial->msSkin->uiTextures].mttType = mttMaterialTypes[i].mttType;
}

void _Material_SetTextureScroll(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	MaterialSkin_t *msSkin;
	MathVector2f_t vScroll;

	// Ensure there's the correct number of arguments being given.
	if (sscanf(cArg, "%f %f", &vScroll[0], &vScroll[1]) != 2)
		// Otherwise throw us an error.
		Sys_Error("Invalid arguments! (%s) (%i)", mCurrentMaterial->cPath, iScriptLine);

	// Get the current skin.
	msSkin = Material_GetSkin(mCurrentMaterial, mCurrentMaterial->iSkins);
	// Apply the scroll variables.
	msSkin->mtTexture[msSkin->uiTextures].vScroll[0] = vScroll[0];
	msSkin->mtTexture[msSkin->uiTextures].vScroll[1] = vScroll[1];

	// Optimisation; let the rendering system let us know to manipulate the matrix for this texture.
	msSkin->mtTexture[msSkin->uiTextures].bManipulated = true;
}

typedef struct
{
	const char *ccVarName;

	VideoTextureEnvironmentMode_t Mode;
} MaterialTextureEnvironmentModeType_t;

MaterialTextureEnvironmentModeType_t tEnvironmentModes[]=
{
	{ "add", VIDEO_TEXTURE_MODE_ADD },
	{ "modulate", VIDEO_TEXTURE_MODE_MODULATE },
	{ "decal", VIDEO_TEXTURE_MODE_DECAL },
	{ "blend", VIDEO_TEXTURE_MODE_BLEND },
	{ "replace", VIDEO_TEXTURE_MODE_REPLACE },
	{ "combine", VIDEO_TEXTURE_MODE_COMBINE }
};

void _Material_SetTextureEnvironmentMode(Material_t *Material, MaterialFunctionType_t Context, char *cArg)
{
	MaterialSkin_t *sCurrentSkin;
	sCurrentSkin = Material_GetSkin(Material, Material->iSkins);
	
	int i;
	for (i = 0; i < pARRAYELEMENTS(tEnvironmentModes); i++)
		if (!strncmp(tEnvironmentModes[i].ccVarName, cArg, Q_strlen(tEnvironmentModes[i].ccVarName)))
		{
			sCurrentSkin->mtTexture[sCurrentSkin->uiTextures].EnvironmentMode = tEnvironmentModes[i].Mode;
			return;
		}

	Con_Warning("Invalid texture environment mode! (%s) (%s)\n", cArg, Material->cName);
}

void _Material_SetRotate(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	MaterialSkin_t	*msSkin;

	// Get the current skin.
	msSkin = Material_GetSkin(mCurrentMaterial, mCurrentMaterial->iSkins);
	// Apply the rotate variable.
	msSkin->mtTexture[msSkin->uiTextures].fRotate = strtof(cArg, NULL);

	// Optimisation; let the rendering system let us know to manipulate the matrix for this texture.
	msSkin->mtTexture[msSkin->uiTextures].bManipulated = true;
}

void _Material_SetBlend(Material_t *currentMaterial, MaterialFunctionType_t context, char *arg)
{

}

// Universal Functions...

typedef struct
{
	int	iFlag;

	const char *ccName;

	MaterialFunctionType_t	mftContext;
} MaterialFlag_t;

MaterialFlag_t	mfMaterialFlags[] =
{
	// Global
	{ MATERIAL_FLAG_PRESERVE, "PRESERVE", MATERIAL_FUNCTION_MATERIAL },
	{ MATERIAL_FLAG_ANIMATED, "ANIMATED", MATERIAL_FUNCTION_MATERIAL },
	{ MATERIAL_FLAG_MIRROR, "MIRROR", MATERIAL_FUNCTION_MATERIAL },
	{ MATERIAL_FLAG_WATER, "WATER", MATERIAL_FUNCTION_MATERIAL },

	// Skin
	{ MATERIAL_FLAG_NEAREST, "NEAREST", MATERIAL_FUNCTION_SKIN },
	{ MATERIAL_FLAG_BLEND, "BLEND", MATERIAL_FUNCTION_SKIN },

	// Texture
	{ MATERIAL_FLAG_ALPHA, "ALPHA", MATERIAL_FUNCTION_TEXTURE }
};

/*	Set flags for the material.
*/
void _Material_SetFlags(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg)
{
	int	i;

	// Search through and copy each flag into the materials list of flags.
	for (i = 0; i < pARRAYELEMENTS(mfMaterialFlags); i++)
	{
		if (strstr(cArg, mfMaterialFlags[i].ccName))
		{
			if (mfMaterialFlags[i].mftContext != mftContext)
				continue;

			switch (mftContext)
			{
			case MATERIAL_FUNCTION_MATERIAL:
				if (mfMaterialFlags[i].iFlag == MATERIAL_FLAG_ANIMATED)
				{
					mCurrentMaterial->iAnimationFrame = 0;
					mCurrentMaterial->dAnimationTime = 0;
				}

				mCurrentMaterial->iFlags |= mfMaterialFlags[i].iFlag;
				break;
			case MATERIAL_FUNCTION_SKIN:
				mCurrentMaterial->msSkin[mCurrentMaterial->iSkins].uiFlags |= mfMaterialFlags[i].iFlag;
				break;
			case MATERIAL_FUNCTION_TEXTURE:
				mCurrentMaterial->msSkin[mCurrentMaterial->iSkins].mtTexture
					[mCurrentMaterial->msSkin[mCurrentMaterial->iSkins].uiTextures].uiFlags |= mfMaterialFlags[i].iFlag;
				break;
			default:
				Con_Warning("Invalid context! (%s) (%s) (%i)\n", mCurrentMaterial->cName, mfMaterialFlags[i].ccName, mftContext);
			}
		}
	}
}

typedef struct
{
	char	*cKey;

	void	(*Function)(Material_t *mCurrentMaterial, MaterialFunctionType_t mftContext, char *cArg);

	MaterialFunctionType_t	mftType;
} MaterialKey_t;

MaterialKey_t mkMaterialFunctions[]=
{
	// Universal
	{ "flags", _Material_SetFlags, MATERIAL_FUNCTION_UNIVERSAL },
	{ "type", _Material_SetType, MATERIAL_FUNCTION_UNIVERSAL },

	// Material
	{ "override_wireframe", _Material_SetWireframe, MATERIAL_FUNCTION_MATERIAL },
	{ "shader", _Material_SetShader, MATERIAL_FUNCTION_MATERIAL },
	{ "animation_speed", _Material_SetAnimationSpeed, MATERIAL_FUNCTION_MATERIAL },
	{ "skin", _Material_AddSkin, MATERIAL_FUNCTION_MATERIAL },
	{ "alpha", _Material_SetAlpha, MATERIAL_FUNCTION_MATERIAL },

	// Skin
	{ "map", _Material_AddTexture, MATERIAL_FUNCTION_SKIN },
	{ "texture", _Material_AddTexture, MATERIAL_FUNCTION_SKIN },

	// Texture
	{ "scroll", _Material_SetTextureScroll, MATERIAL_FUNCTION_TEXTURE },
	{ "rotate", _Material_SetRotate, MATERIAL_FUNCTION_TEXTURE },
	{ "env_mode", _Material_SetTextureEnvironmentMode, MATERIAL_FUNCTION_TEXTURE },
	//{ "blend", _Material_SetBlend, MATERIAL_FUNCTION_TEXTURE },

	{ 0 }
};

void Material_CheckFunctions(Material_t *mNewMaterial)
{
	MaterialKey_t *mKey;

	// Find the related function.
	for (mKey = mkMaterialFunctions; mKey->cKey; mKey++)
		// Remain case sensitive.
		if (!Q_strcasecmp(mKey->cKey, cToken + 1))
		{
			/*	todo
				account for texture slots etc
			*/
			if ((mKey->mftType != MATERIAL_FUNCTION_UNIVERSAL) && (mftMaterialState != mKey->mftType))
				Sys_Error("Attempted to call a function within the wrong context! (%s) (%s) (%i)\n", cToken, mNewMaterial->cPath, iScriptLine);

			Script_GetToken(false);

			mKey->Function(mNewMaterial, mftMaterialState, cToken);
			return;
		}

	Con_Warning("Unknown function! (%s) (%s) (%i)\n", cToken, mNewMaterial->cPath, iScriptLine);
}

/*	Loads and parses material.
	Returns false on complete failure.
*/
Material_t *Material_Load(const char *ccPath)
{
	Material_t  *mNewMaterial;
	void        *cData;
	char		cPath[PLATFORM_MAX_PATH],
				cMaterialName[64] = { 0 };

	// Ensure that the given material names are correct!
	if (ccPath[0] == ' ')
		Sys_Error("Invalid material path! (%s)\n", ccPath);

	if (!bInitialized)
	{
		Con_Warning("Attempted to load material, before initialization! (%s)\n", ccPath);
		return NULL;
	}

	// Update the given path with the base path plus extension.
	sprintf(cPath,"%s%s.material",Global.cMaterialPath,ccPath);

	// Check if it's been cached already...
	mNewMaterial = Material_GetByPath(cPath);
	if(mNewMaterial)
		return mNewMaterial;

	cData = COM_LoadFile(cPath,0);
	if(!cData)
	{
		Con_Warning("Failed to load material! (%s) (%s)\n", cPath, ccPath);
		return NULL;
	}

	Script_StartTokenParsing((char*)cData);

	if(!Script_GetToken(true))
	{
		Con_Warning("Failed to get initial token! (%s) (%i)\n",ccPath,iScriptLine);
		return NULL;
	}
	else if (cToken[0] != '{')
	{
		if (!Q_strcmp(cToken, "material_version"))
		{
			Script_GetToken(false);
		}
		else	// Probably a name...
		{
			// Copy over the given name.
			strncpy(cMaterialName, cToken, sizeof(cMaterialName));
			if (cMaterialName[0] == ' ')
				Sys_Error("Invalid material name!\n");

			// Check if it's been cached already...
			mNewMaterial = Material_GetByName(cMaterialName);
			if (mNewMaterial)
			{
				Con_Warning("Attempted to load duplicate material! (%s) (%s) vs (%s) (%s)\n",
					ccPath, cMaterialName,
					mNewMaterial->cPath, mNewMaterial->cName);

				Z_Free(cData);

				return mNewMaterial;
			}
		}

		Script_GetToken(true);

		if (cToken[0] != '{')
		{
			Con_Warning("Missing '{'! (%s) (%i)\n", ccPath, iScriptLine);

			goto MATERIAL_LOAD_ERROR;
		}
	}

	// Assume that the material hasn't been cached yet, so allocate a new copy of one.
	mNewMaterial = Material_Allocate();
	if (!mNewMaterial)
	{
		Con_Warning("Failed to allocate material! (%s)\n",ccPath);

		goto MATERIAL_LOAD_ERROR;
	}

	if (cMaterialName[0])
		strncpy(mNewMaterial->cName, cMaterialName, sizeof(mNewMaterial->cName));
	else
	{
		char cIn[PLATFORM_MAX_PATH];
		strcpy(cIn, ccPath);

		// Otherwise just use the filename.
		ExtractFileBase(cIn, mNewMaterial->cName);
	}

	strncpy(mNewMaterial->cPath, ccPath, sizeof(mNewMaterial->cPath));

	for (;;)
	{
		if(!Script_GetToken(true))
		{
			Con_Warning("End of field without closing brace! (%s) (%i)\n",ccPath,iScriptLine);

			goto MATERIAL_LOAD_ERROR;
		}

		mftMaterialState = MATERIAL_FUNCTION_MATERIAL;

		// End
		if (cToken[0] == '}')
		{
			// TODO: Load material data at the END!

			return mNewMaterial;
		}
		// Start
		else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
			Material_CheckFunctions(mNewMaterial);
	}

MATERIAL_LOAD_ERROR:
	Z_Free(cData);

	return NULL;
}

bool Material_Precache(const char *ccPath)
{
	Material_t *mNewMaterial;

	mNewMaterial = Material_Load(ccPath);
	if (!mNewMaterial)
	{
		Con_Warning("Failed to load material! (%s)\n", ccPath);
		return false;
	}

	return true;
}

/*
	Rendering
*/

extern ConsoleVariable_t gl_fullbrights;

/*	Typically called before an object is drawn.
*/
void Material_Draw(Material_t *Material, int Skin, 
	VideoObjectVertex_t *ObjectVertex, VideoPrimitive_t ObjectPrimitive, unsigned int ObjectSize,
	bool bPost)
{
	if (r_drawflat_cheatsafe || !Material)
		return;

	if (!Material->bWireframeOverride && (r_lightmap_cheatsafe || r_showtris.bValue))
	{
		if (!bPost)
		{
			// Select the first TMU.
			Video_SelectTexture(0);

			// Set it as white.
			Video_SetTexture(g_mGlobalColour->msSkin[MATERIAL_COLOUR_WHITE].mtTexture->gMap);
		}
		return;
	}

	MaterialSkin_t *msCurrentSkin;
	if (Material->iFlags & MATERIAL_FLAG_ANIMATED)
		msCurrentSkin = Material_GetAnimatedSkin(Material);
	else
		msCurrentSkin = Material_GetSkin(Material, Skin);
	if (!msCurrentSkin)
		Sys_Error("Failed to get valid skin! (%s)\n", Material->cName);

	if (msCurrentSkin->uiFlags & MATERIAL_FLAG_BLEND)
	{
		if (!bPost)
			VideoLayer_Enable(VIDEO_BLEND);
		else
			VideoLayer_Disable(VIDEO_BLEND);
	}

	unsigned int i, uiCurrentUnit;
	for (i = 0, uiCurrentUnit = 0; i < msCurrentSkin->uiTextures; i++, uiCurrentUnit++)
	{
#ifdef VIDEO_LIGHTMAP_HACKS
		// Skip the lightmap, since it's manually handled.
		if (uiCurrentUnit == VIDEO_TEXTURE_LIGHT)
			uiCurrentUnit++;
#endif

		// Attempt to select the unit (if it's already selected, then it'll just return).
		Video_SelectTexture(uiCurrentUnit);

		if (!bPost)
		{
			// Enable it.
			Video_EnableCapabilities(VIDEO_TEXTURE_2D);

			// Bind it.
			Video_SetTexture(msCurrentSkin->mtTexture[i].gMap);

			// Allow us to manipulate the texture.
			if (msCurrentSkin->mtTexture[i].bManipulated)
			{
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				if ((msCurrentSkin->mtTexture[i].vScroll[0] > 0) || (msCurrentSkin->mtTexture[i].vScroll[0] < 0) ||
					(msCurrentSkin->mtTexture[i].vScroll[1] > 0) || (msCurrentSkin->mtTexture[i].vScroll[1] < 0))
					glTranslatef(
						msCurrentSkin->mtTexture[i].vScroll[0] * cl.time,
						msCurrentSkin->mtTexture[i].vScroll[1] * cl.time,
						0);
				if ((msCurrentSkin->mtTexture[i].fRotate > 0) || (msCurrentSkin->mtTexture[i].fRotate < 0))
					glRotatef(msCurrentSkin->mtTexture[i].fRotate*cl.time, 0, 0, 1.0f);
				glMatrixMode(GL_MODELVIEW);
			}
		}

		switch (msCurrentSkin->mtTexture[i].mttType)
		{
		case MATERIAL_TEXTURE_DIFFUSE:
			if (!bPost)
			{
#if 0 // TODO: Material shader assignments!!!!
				VideoShader_SetVariablei(iDiffuseUniform, Video.uiActiveUnit);
#endif

				if (uiCurrentUnit > 0)
				{
					VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_DECAL);

					// Check if we've been given a video object to use...
					if (ObjectVertex)
					{
						unsigned int j;

						// Go through the whole object.
						for (j = 0; j < ObjectSize; j++)
						{
							// Copy over original texture coords.
							Video_ObjectTexture(&ObjectVertex[j], uiCurrentUnit,
								// Use base texture coordinates as a reference.
								ObjectVertex[j].mvST[0][0],
								ObjectVertex[j].mvST[0][1]);
						}
					}
				}
				else
					VideoLayer_SetTextureEnvironmentMode(msCurrentSkin->mtTexture[i].EnvironmentMode);

				if (msCurrentSkin->mtTexture[i].uiFlags & MATERIAL_FLAG_ALPHA)
					VideoLayer_Enable(VIDEO_ALPHA_TEST);
			}
			else
			{
				if (msCurrentSkin->mtTexture[i].uiFlags & MATERIAL_FLAG_ALPHA)
				{
					VideoLayer_Disable(VIDEO_ALPHA_TEST);

					if (cvVideoAlphaTrick.bValue && (ObjectSize > 0))
					{
						Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);

						VideoLayer_Enable(VIDEO_BLEND);

						// Draw the object again (don't bother passing material).
						Video_DrawObject(ObjectVertex, ObjectPrimitive, ObjectSize, NULL, 0);

						VideoLayer_Disable(VIDEO_BLEND);

						Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_TRUE);
					}
				}

				VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_MODULATE);
			}
			break;
		case MATERIAL_TEXTURE_DETAIL:
			if (!bPost)
			{
				if (!cvVideoDrawDetail.bValue)
				{
					Video_DisableCapabilities(VIDEO_TEXTURE_2D);
					break;
				}

				VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_COMBINE);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);

				// Check if we've been given a video object to use...
				if (ObjectVertex)
				{
					unsigned int j;

					// Go through the whole object.
					for (j = 0; j < ObjectSize; j++)
					{
						// Copy over original texture coords.
						Video_ObjectTexture(&ObjectVertex[j], uiCurrentUnit,
							// Use base texture coordinates as a reference.
							ObjectVertex[j].mvST[0][0] * cvVideoDetailScale.value,
							ObjectVertex[j].mvST[0][1] * cvVideoDetailScale.value);

						// TODO: Modify them to the appropriate scale.

					}
				}
			}
			else
			{
				VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_MODULATE);
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
			}
			break;
		case MATERIAL_TEXTURE_FULLBRIGHT:
			if (!bPost)
			{
				if (!gl_fullbrights.bValue)
				{
					Video_DisableCapabilities(VIDEO_TEXTURE_2D);
					break;
				}

				VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_ADD);

				if (uiCurrentUnit > 0)
				{
					// Check if we've been given a video object to use...
					if (ObjectVertex)
					{
						unsigned int j;

						// Go through the whole object.
						for (j = 0; j < ObjectSize; j++)
						{
							// Copy over original texture coords.
							Video_ObjectTexture(&ObjectVertex[j], uiCurrentUnit,
								// Use base texture coordinates as a reference.
								ObjectVertex[j].mvST[0][0],
								ObjectVertex[j].mvST[0][1]);
						}
					}
				}
			}
			else
				VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_MODULATE);
			break;
		case MATERIAL_TEXTURE_SPHERE:
			if (!bPost)
			{
				VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_COMBINE);

				Video_GenerateSphereCoordinates();

				VideoLayer_Enable(VIDEO_TEXTURE_GEN_S | VIDEO_TEXTURE_GEN_T);
			}
			else
			{
				VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_MODULATE);
				VideoLayer_Disable(VIDEO_TEXTURE_GEN_S | VIDEO_TEXTURE_GEN_T);
			}
			break;
		default:
			Sys_Error("Invalid texture type for material! (%s) (%i)\n", Material->cPath, msCurrentSkin->mtTexture[i].mttType);
		}

		if (bPost)
		{
			// Reset any manipulation within the matrix.
			if (msCurrentSkin->mtTexture[i].bManipulated)
			{
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glTranslatef(0, 0, 0);
				glRotatef(0, 0, 0, 0);
				glMatrixMode(GL_MODELVIEW);
			}

			// Disable the texture.
			Video_DisableCapabilities(VIDEO_TEXTURE_2D);
		}
	}
}

/**/

void Material_Shutdown(void)
{
	Con_Printf("Shutting down material system...\n");
}
