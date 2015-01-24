/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "quakedef.h"

/*
	Material System
	This is pretty much written from scratch without using anything else for reference,
	more just for inspiration. This has been a pretty weird learning experience for me
	since I've never written anything out like this before but it works and does the job
	it was designed to do.
	~hogsy
*/

#include "engine_videomaterial.h"

#include "engine_script.h"

bool	bInitialized = false;

Material_t	mMaterials[MATERIAL_MAX];	// Global array.

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
	{	MATERIAL_TYPE_SNOW,		"snow"		},
	{	MATERIAL_TYPE_MUD,		"mud"		}
};

int	iMaterialCount = 0;

Material_t *Material_Allocate(void);

void Material_List(void);

void Material_Initialize(void)
{
	Material_t *mDummy;

	if(bInitialized)
		return;

	Con_Printf("Initializing material system...\n");

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

	for (i = 0; i < MATERIAL_MAX; i++)
	{
		// Probably the end, just break.
		if (!mMaterials[i].iSkins)
			break;

		iSkins += mMaterials[i].iSkins;

		Con_Printf(" %s (%s) (%i)\n", mMaterials[i].cName, mMaterials[i].cPath, mMaterials[i].iSkins);
	}

	Con_Printf("\nListed %i active materials with %i skins in total!\n", i,iSkins);
}

/*
	Management
*/

Material_t *Material_Allocate(void)
{
	int	i;

	// In the case of allocation, we go through the entire array.
	for (i = 0; i < MATERIAL_MAX; i++)
		if (!(mMaterials[i].iFlags & MATERIAL_FLAG_PRESERVE))
			if (!mMaterials[i].iIdentification || !mMaterials[i].iSkins)
			{
				// Set our new material with defaults.
				mMaterials[i].cName[0]			= 0;
				mMaterials[i].iIdentification	= i;
				mMaterials[i].iSkins			= 0;
				mMaterials[i].iFlags			= 0;
				mMaterials[i].bBind				= true;

				iMaterialCount++;

				return &mMaterials[i];
			}

	return NULL;
}

/*	Clears all the currently active materials.
*/
void Material_ClearActive(void)
{
	int	i;

	for (i = 0; i < MATERIAL_MAX; i++)
	{
		if (!(mMaterials[i].iFlags & MATERIAL_FLAG_PRESERVE))
		{ }
	}
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
	if (cArg[0] == '@')
	{
		if (!stricmp(cArg, "@notexture"))
			return notexture;
		else
			Sys_Error("Attempted to set invalid internal texture! (%s)\n", mMaterial->cPath);
	}

	bTextureMap = Image_LoadImage(cArg,
		&mCurrentSkin->iTextureWidth,
		&mCurrentSkin->iTextureHeight);
	if (bTextureMap)
	{
		// Warn about incorrect sizes.
		if ((mCurrentSkin->iTextureWidth & 15) || (mCurrentSkin->iTextureHeight & 15))
		{
			Con_Warning("Texture is not 16 aligned! (%s) (%ix%i)\n", cArg, mCurrentSkin->iTextureWidth, mCurrentSkin->iTextureHeight);
		
			// Pad the image.
			iTextureFlags |= TEXPREF_PAD;
		}

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

void _Material_SetAnimationSpeed(Material_t *mCurrentMaterial, char *cArg)
{
	mCurrentMaterial->iAnimationSpeed = atoi(cArg);
}

typedef struct
{
	int	iFlag;

	const	char	*ccName;

	bool	bGlobal;
} MaterialFlag_t;

MaterialFlag_t	mfMaterialFlags[] =
{
	// Global
	{	MATERIAL_FLAG_PRESERVE,	"PRESERVE",	true	},
	{	MATERIAL_FLAG_ANIMATED,	"ANIMATED",	true	},
	{	MATERIAL_FLAG_MIRROR,	"MIRROR",	true	},
	{	MATERIAL_FLAG_WATER,	"WATER",	true	},

	// Local
	{	MATERIAL_FLAG_ALPHA,	"ALPHA",	false	},
	{	MATERIAL_FLAG_BLEND,	"BLEND",	false	}
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
			{
				if (!mfMaterialFlags[i].bGlobal)
					Con_Warning("Attempted to set a skin flag globally! (%s) (%s)\n", mCurrentMaterial->cName, mfMaterialFlags[i].ccName);
				else
					mCurrentMaterial->iFlags |= mfMaterialFlags[i].iFlag;
			}
			else
			{
				if (mfMaterialFlags[i].bGlobal)
					Con_Warning("Attempted to set a global flag to a skin! (%s) (%s)\n", mCurrentMaterial->cName, mfMaterialFlags[i].ccName);
				else
					mCurrentMaterial->msSkin[mCurrentMaterial->iSkins - 1].iFlags |= mfMaterialFlags[i].iFlag;
			}
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

	// (closer to Quake III naming conventions)
	{ "detail", _Material_SetDetailTexture },
	{ "map", _Material_SetDiffuseTexture },
	{ "animation_speed", _Material_SetAnimationSpeed },

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
Material_t *Material_Load(const char *ccPath)
{
    Material_t  *mNewMaterial;
	void        *cData;
	char		cPath[PLATFORM_MAX_PATH],
				cMaterialName[64] = { 0 };

	// Ensure that the given material names are correct!
	if (ccPath[0] == ' ')
		Sys_Error("Invalid material name! (%s)\n", ccPath);

	Con_DPrintf("Loading material: %s\n", ccPath);

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

			Z_Free(cData);

			return mNewMaterial;
		}

		if (cMaterialName[0])
		{
			Script_GetToken(true);
			if (cToken[0] != '{')
			{
				Con_Warning("Missing '{'! (%s) (%i)\n", ccPath, iScriptLine);

				goto MATERIAL_LOAD_ERROR;
			}
		}
		else
		{
			Con_Warning("Invalid material name! (%s) (%i)\n", ccPath, iScriptLine);

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
		// Copy the name over.
		strncpy(mNewMaterial->cName, cMaterialName, sizeof(mNewMaterial->cName));
#if 0	// This can end up causing unnecessary conflicts, don't bother.
	else
		// Otherwise just use the filename.
		ExtractFileBase(ccPath, mNewMaterial->cName);
#endif

	// Copy the path over.
	strncpy(mNewMaterial->cPath, ccPath, sizeof(mNewMaterial->cPath));

	while(true)
	{
		if(!Script_GetToken(true))
		{
			Con_Warning("End of field without closing brace! (%s) (%i)\n",ccPath,iScriptLine);

			goto MATERIAL_LOAD_ERROR;
		}

		// End
		if (cToken[0] == '}')
			return mNewMaterial;
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

MATERIAL_LOAD_ERROR:
	Z_Free(cData);

	return NULL;
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