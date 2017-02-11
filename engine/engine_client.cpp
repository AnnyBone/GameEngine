/*
Copyright (C) 2011-2016 OldTimes Software

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

#include "engine_base.h"

#include "engine_client.h"
#include "video.h"

#include "shared_server.h"
#include "shared_menu.h"

using namespace core;

namespace core
{
	class ClientManager : public XManager
	{
	};
}

ClientManager *g_clientmanager = nullptr;

/*	Client C Interface	*/

extern "C" entity_t *Client_GetPlayerEntity(void)
{
	return &cl_entities[cl.viewentity];
}

extern "C" entity_t *Client_GetViewEntity(void)
{
	return &cl.viewent;
}

/*	Gets an effect texture.
*/
extern "C" int Client_GetEffect(const char *cPath)
{
	int i;

	for(i = 0; i < MAX_EFFECTS; i++)
		if (!g_effecttextures[i])
		{
			Con_Warning("Failed to find effect texture! (%s)\n",cPath);
			break;
		}
		else if (!strcmp(cPath, g_effecttextures[i]->name))
			return i;

	return 0;
}

/*	Precache client-side resources.
*/
extern "C" void Client_PrecacheResource(int type, const char *resource) {
	char			cPath[PL_SYSTEM_MAX_PATH];
	int				i;
	unsigned int    w,h;
	model_t			*mClientModel;
	uint8_t			*bData;

	switch(type)
	{
	case RESOURCE_FONT:
		break;
	case RESOURCE_MODEL:
		for(i = 0; i < MAX_MODELS; i++)
			if(!cl.model_precache[i])
			{
				mClientModel = Mod_ForName(resource);
				if(!mClientModel)
				{
					Console_ErrorMessage(false,resource,"Either the file is corrupt or does not exist.");
					return;
				}

				cl.model_precache[i] = mClientModel;
				return;
			}
			else if(!strcmp(cl.model_precache[i]->name,resource))
				return;

		Console_ErrorMessage(false,resource,"Overflow!");
		break;
	// [26/1/2013] Precache for effect types ~hogsy
	case RESOURCE_SPRITE:
		sprintf(cPath,PATH_SPRITES"%s",resource);

		for(i = 0; i < MAX_EFFECTS; i++)
			if (!g_effecttextures[i])
			{
				bData = Image_LoadImage(cPath,&w,&h);
				if(!bData)
				{
					Con_Warning("Failed to load %s!\n",cPath);
					return;
				}

				g_effecttextures[i] = TexMgr_LoadImage(NULL, resource, w, h, SRC_RGBA, bData, cPath, 0, TEXPREF_ALPHA);
				return;
			}
			else if (!strcmp(g_effecttextures[i]->name, resource))
				return;

		Console_ErrorMessage(false,cPath,"Overflow!");
		break;
	case RESOURCE_MATERIAL:
		// TODO: Implement material caching support.
		break;
	case RESOURCE_TEXTURE:
		break;
	default:
		Con_Warning("Attempted to precache an undefined type! (%s)\n",resource);
	}
}

extern "C" int Client_GetStat(ClientStat_t csStat)
{
	return cl.stats[csStat];
}
