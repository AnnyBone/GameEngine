/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "quakedef.h"

#include "engine_videomaterial.h"
#include "engine_video.h"

#include "engine_script.h"

bool	bInitialized = false;

Material_t	mMaterials[MATERIALS_MAX_ALLOCATED];	// Global array.

MaterialType_t	MaterialTypes[]=
{
	{	MATERIAL_TYPE_NONE,		"default"	},
	{	MATERIAL_TYPE_METAL,	"metal"		},
	{	MATERIAL_TYPE_GLASS,	"glass"		},
	{	MATERIAL_TYPE_CONCRETE,	"concrete"	},
	{	MATERIAL_TYPE_WOOD,		"wood"		},
	{	MATERIAL_TYPE_DIRT,		"dirt"		},
	{	MATERIAL_TYPE_RUBBER,	"rubber"	},
	{	MATERIAL_TYPE_WATER,	"water"		},
	{	MATERIAL_TYPE_FLESH,	"flesh"		},
	{	MATERIAL_TYPE_MUD,		"mud"		}
};

int	iMaterialCount = 0;

cvar_t	cvMaterialDraw			= { "material_draw",		"1", false, false, "Enables and disables the drawing of materials."		},
		cvMaterialDrawDetail	= {	"material_drawdetail",	"1", false, false, "Enables and disables the drawing of detail maps."	};

Material_t *Material_Allocate(void);

void Material_List(void);

void Material_Initialize(void)
{
	Material_t *mDummy;

	if(bInitialized)
		return;

	Con_Printf("Initializing material system...\n");

	Cvar_RegisterVariable(&cvMaterialDraw, NULL);
	Cvar_RegisterVariable(&cvMaterialDrawDetail, NULL);

	Cmd_AddCommand("material_list", Material_List);

	// Must be set to initialized before anything else.
	bInitialized = true;

	// Add dummy material.
	mDummy = Material_Load("engine/notexture");
	if (!mDummy)
		Sys_Error("Failed to create dummy material!\n");

#ifdef _MSC_VER // This is false, since the function above shuts us down, but MSC doesn't understand that.
#pragma warning(suppress: 6011)
#endif
}

/*	Lists all the currently active materials.
*/
void Material_List(void)
{
	int i,iSkins = 0;

	Con_Printf("Listing materials...\n");

	for (i = 0; i < MATERIALS_MAX_ALLOCATED; i++)
	{
		// Probably the end, just break.
		if (!mMaterials[i].iSkins)
			break;

		iSkins += mMaterials[i].iSkins;

		Con_Printf(" %s (%s) (%i)\n", mMaterials[i].cName, mMaterials[i].cPath, mMaterials[i].iSkins);
	}

	Con_Printf("\nListed %i active materials with %i skins in total!\n", i,iSkins);
}

Material_t *Material_Allocate(void)
{
	int	i;

	// In the case of allocation, we go through the entire array.
	for (i = 0; i < MATERIALS_MAX_ALLOCATED; i++)
		if (!(mMaterials[i].iFlags & MATERIAL_FLAG_PRESERVE))
			if (!mMaterials[i].iIdentification || !mMaterials[i].iSkins)
			{
				// Set our new material with defaults.
				mMaterials[i].iIdentification	= i;
				mMaterials[i].iSkins			= 0;
				mMaterials[i].iFlags			= 0;

				iMaterialCount++;

				return &mMaterials[i];
			}

	return NULL;
}

MaterialSkin_t *Material_GetSkin(Material_t *mMaterial,int iSkin)
{
	// Don't let us spam the console; silly but whatever.
	static	int	iPasses = 0;

	if(iSkin < 0 || iSkin > MODEL_MAX_TEXTURES)
	{
		if (iPasses < 50)
		{
			Con_Warning("Invalid skin identification, should be greater than 0 and less than %i! (%i)\n", MODEL_MAX_TEXTURES, iSkin);
			iPasses++;
		}
		return NULL;
	}
	else if(!mMaterial)
	{
		if (iPasses < 50)
		{
			Con_Warning("Invalid material!\n");
			iPasses++;
		}
		return NULL;
	}
	else if(!mMaterial->iSkins)
	{
		if (iPasses < 50)
		{
			Con_Warning("Material with no valid skins! (%s)\n", mMaterial->cName);
			iPasses++;
		}
		return NULL;
	}
	else if (iSkin > (mMaterial->iSkins - 1))
	{
		if (iPasses < 50)
		{
			Con_Warning("Attempted to get an invalid skin! (%i) (%s)\n", iSkin, mMaterial->cName);
			iPasses++;
		}
		return NULL;
	}

	return &mMaterial->msSkin[iSkin];
}

/*	Returns a material from the given ID.
*/
Material_t *Material_Get(int iMaterialID)
{
	int i;

	// The identification would never be less than 0, and never more than our maximum.
	if(iMaterialID < 0 || iMaterialID > MATERIALS_MAX_ALLOCATED)
	{
		Con_Warning("Invalid material ID! (%i)\n",iMaterialID);
		return NULL;
	}

	for (i = 0; i < iMaterialCount; i++)
		if(mMaterials[i].iIdentification == iMaterialID)
			return &mMaterials[i];

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

extern cvar_t gl_fullbrights;

void Material_PreDraw(Material_t *mMaterial, int iSkin, VideoObject_t *voObject, int iSize)
{
	int				i, iLayers = 1;
	MaterialSkin_t	*msCurrentSkin;

	if (!cvMaterialDraw.bValue || !mMaterial)
		return;
	// If we're drawing flat, then don't apply textures.
	else if (r_drawflat_cheatsafe)
	{
		Video_DisableCapabilities(VIDEO_TEXTURE_2D);
		return;
	}

	msCurrentSkin = Material_GetSkin(mMaterial, iSkin);
	if (!msCurrentSkin)
	{
		Video_SetTexture(notexture);
		return;
	}

	if (msCurrentSkin->iFlags & MATERIAL_FLAG_ALPHA)
		Video_EnableCapabilities(VIDEO_ALPHA_TEST);

	// Set the diffuse texture first.
	Video_SetTexture(msCurrentSkin->gDiffuseTexture);

	if (msCurrentSkin->gLightmapTexture)
	{
		Video_SelectTexture(iLayers);
		Video_EnableCapabilities(VIDEO_TEXTURE_2D);
		Video_SetTexture(msCurrentSkin->gLightmapTexture);

		// Overbrights
#if 0
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
#else
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
#endif

		iLayers++;
	}

	// Detail map layer.
	if (msCurrentSkin->gDetailTexture && cvMaterialDrawDetail.bValue)
	{
		// TODO: Check distance from camera before proceeding.

		Video_SelectTexture(iLayers);
		Video_EnableCapabilities(VIDEO_TEXTURE_2D | VIDEO_BLEND);
		Video_SetTexture(msCurrentSkin->gDetailTexture);

		if (voObject)
			for (i = 0; i < iSize; i++)
			{
				// Copy over original texture coords.
				voObject[i].vTextureCoord[iLayers][0] = voObject[i].vTextureCoord[0][0] * 8;
				voObject[i].vTextureCoord[iLayers][1] = voObject[i].vTextureCoord[0][1] * 8;

				// TODO: Modify them to the appropriate scale.

			}

		iLayers++;
	}

#if 0	// TODO: Finish implementing this at some point... Meh ~hogsy
	if (msCurrentSkin->gSpecularTexture)
	{
		Video_SelectTexture(iLayers);
		Video_SetTexture(msCurrentSkin->gSpecularTexture);
		Video_EnableCapabilities(VIDEO_TEXTURE_2D | VIDEO_BLEND);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);

		iLayers++;
	}
#endif

	// Fullbright map.
	if (msCurrentSkin->gFullbrightTexture && gl_fullbrights.bValue)
	{
		Video_SelectTexture(iLayers);
		Video_EnableCapabilities(VIDEO_TEXTURE_2D);
		Video_SetTexture(msCurrentSkin->gFullbrightTexture);

		if (voObject)
			for (i = 0; i < iSize; i++)
			{
				// Texture coordinates remain the same for fullbright layers.
				voObject[i].vTextureCoord[iLayers][0] = voObject[i].vTextureCoord[0][0];
				voObject[i].vTextureCoord[iLayers][1] = voObject[i].vTextureCoord[0][1];
			}

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

		iLayers++;
	}

	// Sphere map.
	if (msCurrentSkin->gSphereTexture)
	{
		Video_SelectTexture(iLayers);
		Video_SetTexture(msCurrentSkin->gSphereTexture);
		Video_GenerateSphereCoordinates();
		Video_EnableCapabilities(VIDEO_TEXTURE_2D | VIDEO_BLEND | VIDEO_TEXTURE_GEN_S | VIDEO_TEXTURE_GEN_T);

#if 0
		if (msCurrentSkin->gSpecularTexture)
			Video_SetBlend(VIDEO_BLEND_ONE, VIDEO_DEPTH_IGNORE);
#endif

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

		iLayers++;
	}
}

void Material_PostDraw(Material_t *mMaterial, int iSkin, VideoObject_t *voObject, unsigned int uiSize)
{
	MaterialSkin_t	*msCurrentSkin;

	if (!cvMaterialDraw.bValue || !mMaterial)
		return;
	// If we're drawing flat, then don't apply textures.
	else if (r_drawflat_cheatsafe)
		return;

	msCurrentSkin = Material_GetSkin(mMaterial, iSkin);
	if (!msCurrentSkin)
		return;

	if ((msCurrentSkin->iFlags & MATERIAL_FLAG_ALPHA) && cvVideoAlphaTrick.bValue)
	{
		Video_SelectTexture(0);

		Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);

		Video_EnableCapabilities(VIDEO_BLEND);

		/*	HACKY
			We enabled this above, so after resetting we would normally then disable this,
			but disabling it here undoes that and then causes the pipeline to instead enable
			it again. Not wanted behaviour, so we just ignore it.
		*/
		bVideoIgnoreCapabilities = true;
		Video_DisableCapabilities(VIDEO_ALPHA_TEST);
		bVideoIgnoreCapabilities = false;

		// Draw the object again (don't bother passing material).
		Video_DrawObject(voObject, VIDEO_PRIMITIVE_TRIANGLE_FAN, uiSize, NULL, 0);
	}
}

/*
	Scripting
*/

bool	bMaterialGlobal;	// Indicates that any settings applied are global.

// Utility functions...

gltexture_t *Material_LoadTexture(Material_t *mMaterial,MaterialSkin_t *mCurrentSkin, char *cArg)
{
	int		iTextureFlags = TEXPREF_ALPHA;
	byte	*bTextureMap;

	// Check if it's trying to use a built-in texture.
	if (!Q_strcmp(cArg, "notexture"))
		return notexture;

	bTextureMap = Image_LoadImage(cArg,
		&mCurrentSkin->iTextureWidth,
		&mCurrentSkin->iTextureHeight);
	if (bTextureMap)
	{
		// Warn about incorrect sizes.
		if ((mCurrentSkin->iTextureWidth & 15) || (mCurrentSkin->iTextureHeight & 15))
			Con_Warning("Texture is not 16 aligned! (%ix%i)\n", mCurrentSkin->iTextureWidth, mCurrentSkin->iTextureHeight);

		if (mMaterial->iFlags & MATERIAL_FLAG_PRESERVE)
			iTextureFlags |= TEXPREF_PERSIST;

		return TexMgr_LoadImage(NULL,cArg,
			mCurrentSkin->iTextureWidth,
			mCurrentSkin->iTextureHeight,
			SRC_RGBA,bTextureMap,cArg,0,iTextureFlags);
	}
		
	Con_Warning("Failed to load texture %s!\n", cArg);

	return notexture;
}

// Everything else...

void _Material_SetType(Material_t *mCurrentMaterial,char *cArg)
{
	int	iMaterialType = Q_atoi(cArg);

	// Ensure that the given type is valid.
	if((iMaterialType < MATERIAL_TYPE_NONE) || (iMaterialType >= MATERIAL_TYPE_MAX))
		Con_Warning("Invalid material type! (%i)\n",iMaterialType);

	mCurrentMaterial->msSkin[mCurrentMaterial->iSkins-1].iType = iMaterialType;
}

void _Material_SetDiffuseTexture(Material_t *mCurrentMaterial,char *cArg)
{
	mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1].gDiffuseTexture = Material_LoadTexture(mCurrentMaterial,&mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1], cArg);
}

void _Material_SetFullbrightTexture(Material_t *mCurrentMaterial,char *cArg)
{
	mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1].gFullbrightTexture = Material_LoadTexture(mCurrentMaterial,&mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1], cArg);
}

void _Material_SetSphereTexture(Material_t *mCurrentMaterial,char *cArg)
{
	mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1].gSphereTexture = Material_LoadTexture(mCurrentMaterial,&mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1], cArg);
}

void _Material_SetSpecularTexture(Material_t *mCurrentMaterial, char *cArg)
{
	mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1].gSpecularTexture = Material_LoadTexture(mCurrentMaterial,&mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1], cArg);
}

void _Material_SetDetailTexture(Material_t *mCurrentMaterial, char *cArg)
{
	mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1].gDetailTexture = Material_LoadTexture(mCurrentMaterial,&mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1], cArg);
}

typedef struct
{
	int	iFlag;

	const	char	*ccName;
} MaterialFlag_t;

MaterialFlag_t	mfMaterialFlags[] =
{
	{ MATERIAL_FLAG_PRESERVE,	"PRESERVE"	},
	{ MATERIAL_FLAG_ALPHA,		"ALPHA"		},
	{ MATERIAL_FLAG_BLEND,		"BLEND"		}
};

/*	Set flags for the material.
*/
void _Material_SetFlags(Material_t *mCurrentMaterial,char *cArg)
{
	int	i;

	// Search through and copy each flag into the materials list of flags.
	for (i = 0; i < pARRAYELEMENTS(mfMaterialFlags); i++)
		if (strstr(cArg, mfMaterialFlags[i].ccName))
		{
			if (bMaterialGlobal)
				mCurrentMaterial->iFlags |= mfMaterialFlags[i].iFlag;
			else
				mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1].iFlags |= mfMaterialFlags[i].iFlag;
		}
}

typedef struct
{
	char	*cKey;

	void	(*Function)(Material_t *mCurrentMaterial,char *cArg);
} MaterialKey_t;

MaterialKey_t	mkMaterialFunctions[]=
{
	{	"SetType",				_Material_SetType				},	// Sets the type of material.
	{	"SetDiffuseTexture",	_Material_SetDiffuseTexture		},	// Sets the diffuse texture.
	{	"SetSpecularTexture",	_Material_SetSpecularTexture	},	// Sets the specular map.
	{	"SetSphereTexture",		_Material_SetSphereTexture		},	// Sets the spheremap texture.
	{	"SetFullbrightTexture",	_Material_SetFullbrightTexture	},	// Sets the fullbright texture.
	{	"SetDetailTexture",		_Material_SetDetailTexture		},	// Sets the detail texture.
	{	"SetFlags",				_Material_SetFlags				},	// Sets seperate flags for the material; e.g. persist etc.

	{	0	}
};

void Material_CheckFunctions(Material_t *mNewMaterial)
{
	MaterialKey_t *mKey;

	// Find the related function.
	for (mKey = mkMaterialFunctions; mKey->cKey; mKey++)
		// Remain case sensitive.
		if (!Q_strcasecmp(mKey->cKey, cToken + 1))
		{
			Script_GetToken(false);

			mKey->Function(mNewMaterial, cToken);
			break;
		}
}

/*	Loads and parses material.
	Returns false on complete failure.
*/
Material_t *Material_Load(/*const */char *ccPath)
{
    Material_t  *mNewMaterial;
	byte        *cData;
	char		cPath[PLATFORM_MAX_PATH],
				cMaterialName[64] = { 0 };

	// Ensure that the given material names are correct!
	if (ccPath[0] == ' ')
		Sys_Error("Invalid texture name! (%s)\n", ccPath);

	Con_DPrintf("Loading material: %s\n", ccPath);

	if(!bInitialized)
	{
		Con_Warning("Attempted to load material, before initialization! (%s)\n",ccPath);
		return NULL;
	}

	// Update the given path with the base path plus extension.
	sprintf(cPath,"%s%s.material",Global.cMaterialPath,ccPath);

	// Check if it's been cached already...
	mNewMaterial = Material_GetByPath(cPath);
	if(mNewMaterial)
		return mNewMaterial;
	
	cData = COM_LoadTempFile(cPath);
	if(!cData)
	{
		Con_Warning("Failed to load material! (%s) (%s)\n", cPath, ccPath);
		return NULL;
	}

	Script_StartTokenParsing((char*)cData);

	bMaterialGlobal = true;

	if(!Script_GetToken(true))
	{
		Con_Warning("Failed to get initial token! (%s) (%i)\n",ccPath,iScriptLine);
		return NULL;
	}
	else if (cToken[0] != '{')
	{
		// Copy over the given name.
		strncpy(cMaterialName, cToken, sizeof(cMaterialName));

		// Check if it's been cached already...
		mNewMaterial = Material_GetByName(cMaterialName);
		if (mNewMaterial)
		{
			Con_Warning("Attempted to load duplicate material! (%s) (%s) vs (%s) (%s)\n",
				ccPath, cMaterialName,
				mNewMaterial->cPath, mNewMaterial->cName);
			return mNewMaterial;
		}

		if (cMaterialName[0])
		{
			Script_GetToken(true);
			if (cToken[0] != '{')
			{
				Con_Warning("Missing '{'! (%s) (%i)\n", ccPath, iScriptLine);
				return NULL;
			}
		}
		else
		{
			Con_Warning("Invalid material name! (%s) (%i)\n", ccPath, iScriptLine);
			return NULL;
		}
	}

	// Assume that the material hasn't been cached yet, so allocate a new copy of one.
	mNewMaterial = Material_Allocate();
	if (!mNewMaterial)
	{
		Con_Warning("Failed to allocate material! (%s)\n",ccPath);
		return NULL;
	}

	if (cMaterialName[0])
		// Copy the name over.
		strncpy(mNewMaterial->cName, cMaterialName, sizeof(mNewMaterial->cName));
	else
		// Otherwise just use the filename.
		ExtractFileBase(ccPath, mNewMaterial->cName);

	// Copy the path over.
	strncpy(mNewMaterial->cPath, ccPath, sizeof(mNewMaterial->cPath));

	while(true)
	{
		if(!Script_GetToken(true))
		{
			Con_Warning("End of field without closing brace! (%s) (%i)\n",ccPath,iScriptLine);
			break;
		}

		// End
		if (cToken[0] == '}')
			break;
		// Start
		else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
			Material_CheckFunctions(mNewMaterial);
		else if(cToken[0] == '{')
		{
			bMaterialGlobal = false;

			mNewMaterial->iSkins++;

			while(true)
			{
				if(!Script_GetToken(true))
				{
					Con_Warning("End of field without closing brace! (%s) (%i)\n",ccPath,iScriptLine);
					break;
				}

				if (cToken[0] == '}')
					break;
				// '$' declares that the following is a function.
				else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
					Material_CheckFunctions(mNewMaterial);
				// '%' declares that the following is a variable.
				else if(cToken[0] == SCRIPT_SYMBOL_VARIABLE)
				{
					/*	TODO:
							* Collect variable
							* Check it against internal solutions
							* Otherwise declare it, figure out where/how it's used
					*/
				}
				else
				{
					Con_Warning("Invalid field! (%s) (%i)\n", ccPath, iScriptLine);
					break;
				}
			}
		}
	}

	return mNewMaterial;
}

/*	Returns default dummy material.
*/
Material_t *Material_GetDummy(void)
{
	Material_t *mDummy;

	mDummy = Material_GetByName("notexture");
	if (!mDummy)
		Sys_Error("Failed to assign dummy material!\n");

	return mDummy;
}

/**/

void Material_Shutdown(void)
{
	Con_Printf("Shutting down material system...\n");
}