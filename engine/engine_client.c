/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "engine_client.h"

/*
	Client-side stuff!
*/

#include "EngineVideo.h"
#include "engine_console.h"

#include "shared_server.h"
#include "shared_menu.h"

/*
	Utilities
*/

entity_t *Client_GetPlayerEntity(void)
{
	return &cl_entities[cl.viewentity];
}

entity_t *Client_GetViewEntity(void)
{
	return &cl.viewent;
}

/*	Gets an effect texture.
*/
int	Client_GetEffect(const char *cPath)
{
	int i;

	for(i = 0; i < MAX_EFFECTS; i++)
		if(!gEffectTexture[i])
		{
			Con_Warning("Failed to find effect texture! (%s)\n",cPath);
			break;
		}
		else if(!strcmp(cPath,gEffectTexture[i]->name))
			return i;

	return 0;
}

// [30/7/2013] "Fixed" so that VS doesn't give us any trouble ~hogsy
gltexture_t	*gMenuTexture[128];

/*	Precache client-side resources.
*/
void Client_PrecacheResource(int iType,char *cResource)
{
	char	cPath[MAX_OSPATH];
	int		i;
	unsigned int    w,h;
	model_t	*mClientModel;
	uint8_t	*bData;

	switch(iType)
	{
	case RESOURCE_FONT:
		break;
	case RESOURCE_MODEL:
		for(i = 0; i < MAX_MODELS; i++)
			if(!cl.model_precache[i])
			{
				mClientModel = Mod_ForName(cResource);
				if(!mClientModel)
				{
					Console_ErrorMessage(false,cResource,"Either the file is corrupt or does not exist.");
					return;
				}

				cl.model_precache[i] = mClientModel;
				return;
			}
			else if(!strcmp(cl.model_precache[i]->name,cResource))
				return;

		Console_ErrorMessage(false,cResource,"Overflow!");
		break;
	// [26/1/2013] Precache for effect types ~hogsy
	case RESOURCE_SPRITE:
		sprintf(cPath,PATH_SPRITES"%s",cResource);

		for(i = 0; i < MAX_EFFECTS; i++)
			if(!gEffectTexture[i])
			{
				bData = Image_LoadImage(cPath,&w,&h);
				if(!bData)
				{
					Con_Warning("Failed to load %s!\n",cPath);
					return;
				}

				gEffectTexture[i] = TexMgr_LoadImage(NULL,cResource,w,h,SRC_RGBA,bData,cPath,0,TEXPREF_ALPHA);
				return;
			}
			else if (!strcmp(gEffectTexture[i]->name,cResource))
				return;

		Console_ErrorMessage(false,cPath,"Overflow!");
		break;
	case RESOURCE_MATERIAL:
		// TODO: Implement material caching support.
		break;
	case RESOURCE_TEXTURE:
		for(i = 0; i < sizeof(gMenuTexture); i++)
			if(!gMenuTexture[i])
			{
				bData = Image_LoadImage(cResource,&w,&h);
				if(!bData)
				{
					Con_Warning("Failed to load %s!\n",cResource);
					return;
				}

				gMenuTexture[i] = TexMgr_LoadImage(NULL,cResource,w,h,SRC_RGBA,bData,cResource,0,TEXPREF_ALPHA);
				return;
			}
			else if(!strcmp(cResource,gMenuTexture[i]->name))
				return;

		Console_ErrorMessage(false,cResource,"Overflow!");
		break;
	default:
		Con_Warning("Attempted to precache an undefined type! (%s)\n",cResource);
	}
}

int Client_GetStat(ClientStat_t csStat)
{
	return cl.stats[csStat];
}
