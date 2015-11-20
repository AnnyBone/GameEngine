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

#include "EngineGame.h"
#include "video.h"
#include "engine_client.h"
#include "EngineMenu.h"
#include "EngineServer.h"
#include "EngineVideoParticle.h"

#include "SharedModule.h"

/*
	Game Interface

	TODO:
		Move Server_ functions into KatServer
*/

pINSTANCE hGameInstance;

GameExport_t *Game;

ServerEntity_t *Game_GetEdicts(void)
{
	return sv.edicts;
}

int Game_GetNumEdicts(void)
{
	return sv.num_edicts;
}

void Server_ChangeLevel(const char *ccNewLevel)
{
	// Make sure we don't issue two changelevels
	if(svs.bChangingLevel)
		return;

	svs.bChangingLevel = true;

	Cbuf_AddText(va("changelevel %s\n",ccNewLevel));
}

char *Server_GetLevelName(void)
{
	return sv.name;
}

ServerEntity_t *Server_FindRadius(MathVector3f_t origin, float radius)
{
	int		i,j;
	ServerEntity_t *eEntity, *eChain;
	MathVector3f_t	eorg;

	eChain = sv.edicts;

	eEntity = NEXT_EDICT(sv.edicts);
	for(i = 1; i < sv.num_edicts; i++,eEntity = NEXT_EDICT(eEntity))
	{
		if(eEntity->free && eEntity->Physics.iSolid == SOLID_NOT)
			continue;

		for(j = 0; j < 3; j++)
			eorg[j] = origin[j]-(eEntity->v.origin[j]+(eEntity->v.mins[j]+eEntity->v.maxs[j])*0.5f);

		if(Math_Length(eorg) > radius)
			continue;

		eEntity->v.chain	= eChain;
		eChain				= eEntity;
	}

	return eChain;
}

int SV_ModelIndex(char *name);

void Server_MakeStatic(ServerEntity_t *ent)
{
	int	i,bits=0;

	if(ent->alpha == ENTALPHA_ZERO)
	{
		ED_Free(ent);
		return;
	}

	if(SV_ModelIndex(ent->v.model) & 0xFF00)
		bits |= B_LARGEMODEL;
	if((int)(ent->v.frame) & 0xFF00)
		bits |= B_LARGEFRAME;
	if(ent->alpha != ENTALPHA_DEFAULT)
		bits |= B_ALPHA;

	if(bits)
	{
		MSG_WriteByte(&sv.signon, SVC_SPAWNSTATIC2);
		MSG_WriteByte(&sv.signon, bits);
	}
	else
		MSG_WriteByte(&sv.signon, svc_spawnstatic);

	if(bits & B_LARGEMODEL)
		MSG_WriteShort(&sv.signon, SV_ModelIndex(ent->v.model));
	else
		MSG_WriteByte(&sv.signon, SV_ModelIndex(ent->v.model));

	if(bits & B_LARGEFRAME)
		MSG_WriteShort(&sv.signon,ent->v.frame);
	else
		MSG_WriteByte(&sv.signon,ent->v.frame);

	MSG_WriteByte(&sv.signon,ent->Model.fScale);
	MSG_WriteByte(&sv.signon,ent->v.colormap);
	MSG_WriteByte(&sv.signon,ent->Model.iSkin);
	for (i=0 ; i<3 ; i++)
	{
		MSG_WriteCoord(&sv.signon, ent->v.origin[i]);
		MSG_WriteAngle(&sv.signon, ent->v.angles[i]);
	}

	if (bits & B_ALPHA)
		MSG_WriteByte (&sv.signon, ent->alpha);

	ED_Free (ent);
}

/*	Sets the model for the specified entity.
*/
void Server_SetModel(ServerEntity_t *ent, char *m)
{
	char	**check;
	model_t	*mod;
	int		i;

	for(i = 0,check = sv.model_precache; *check; i++,check++)
		if(!strcmp(*check, m))
			break;

	// [27/1/2013] Removed dynamic caching. This should only be done client-side derp! ~hogsy
	if(!*check)
		Console_ErrorMessage(true,m,"Model was not registered.");

	ent->v.model		= m;
	ent->v.modelindex	= i;

	mod = sv.models[ent->v.modelindex];
	// [13/4/2013] Oops, made this more secure! ~hogsy
	if(mod && mod->mType == MODEL_TYPE_BSP)
		Game->Server_SetSizeVector(ent,mod->clipmins,mod->clipmaxs);
	else
		Game->Server_SetSizeVector(ent,g_mvOrigin3f,g_mvOrigin3f);
}

void Game_AmbientSound(vec_t *vPosition,const char *cPath,int iVolume,int iAttenuation)
{
	char		**cCheck;
	int			i,iSoundNumber;
	bool		bLarge = false;

	for(iSoundNumber = 0,cCheck = sv.sound_precache; *cCheck; cCheck++,iSoundNumber++)
		if(!strcmp(*cCheck,cPath))
			break;

	if(!*cCheck)
		Console_ErrorMessage(false,(char*)cPath,"Sound was not registered.");

	if(iSoundNumber > 255)
		bLarge = true;

	if(bLarge)
		MSG_WriteByte(&sv.signon,svc_spawnstaticsound2);
	else
		MSG_WriteByte(&sv.signon,svc_spawnstaticsound);

	for(i = 0; i < 3; i++)
		MSG_WriteCoord(&sv.signon,vPosition[i]);

	if(bLarge)
		MSG_WriteShort(&sv.signon,iSoundNumber);
	else
		MSG_WriteByte(&sv.signon,iSoundNumber);

	MSG_WriteByte(&sv.signon,iVolume);
	MSG_WriteByte(&sv.signon,iAttenuation*64);
}

// [18/7/2012] Renamed to Server_Sound ~hogsy
void Server_Sound(ServerEntity_t *ent, int channel, char *sample, int iVolume, float attenuation)
{
	int sound_num, field_mask, i, e;

	if(!sample)
	{
		Con_Warning("Bad sample name (%s)!\n",ent->v.cClassname);
		return;
	}
	else if(iVolume < 0 || iVolume > 255)
	{
		Con_Warning("Sound: volume = %i\n", iVolume);
		return;
	}
	else if(attenuation < 0 || attenuation > 4)
	{
		Con_Warning("Sound: attenuation = %f\n", attenuation);
		return;
	}
	else if(channel < 0 || channel > 7)
	{
		Con_Warning("Sound: channel = %i\n", channel);
		return;
	}
	else if(sv.datagram.cursize > MAX_DATAGRAM-16)
		return;

	for(sound_num=1 ; sound_num<MAX_SOUNDS && sv.sound_precache[sound_num] ; sound_num++)
		if(!strcmp(sample,sv.sound_precache[sound_num]))
			break;

	if(sound_num == MAX_SOUNDS || !sv.sound_precache[sound_num])
	{
		// [19/8/2012] Just return a simple warning ~hogsy
		Con_Warning("%s was not precached!\n",sample);
		return;
	}

	e = NUM_FOR_EDICT(ent);

	field_mask = 0;
	if(iVolume != DEFAULT_SOUND_PACKET_VOLUME)
		field_mask |= SND_VOLUME;

	if(attenuation != DEFAULT_SOUND_PACKET_ATTENUATION)
		field_mask |= SND_ATTENUATION;

	if(e >= 8192)
		field_mask |= SND_LARGEENTITY;

	if(sound_num >= 256 || channel >= 8)
		field_mask |= SND_LARGESOUND;

	MSG_WriteByte(&sv.datagram,svc_sound);
	MSG_WriteByte(&sv.datagram,field_mask);
	if(field_mask & SND_VOLUME)
		MSG_WriteByte(&sv.datagram, iVolume);
	if(field_mask & SND_ATTENUATION)
		MSG_WriteByte(&sv.datagram,attenuation*64);

	if(field_mask & SND_LARGEENTITY)
	{
		MSG_WriteShort(&sv.datagram,e);
		MSG_WriteByte(&sv.datagram,channel);
	}
	else
		MSG_WriteShort(&sv.datagram,(e<<3)|channel);

	if(field_mask & SND_LARGESOUND)
		MSG_WriteShort(&sv.datagram,sound_num);
	else
		MSG_WriteByte(&sv.datagram,sound_num);

	for(i=0 ; i<3 ; i++)
		MSG_WriteCoord(&sv.datagram,ent->v.origin[i]+0.5*(ent->v.mins[i]+ent->v.maxs[i]));
}

/*
	Flares
*/

void Server_Flare(MathVector3f_t org,float r,float g,float b,float a,float scale,char *texture)
{
#if 0
	int		i;
	char	name[PLATFORM_MAX_PATH];

	if(!texture)
	{
		Con_Warning("Tried to spawn a flare with no texture.\n");
		return;
	}
	else if(sv.datagram.cursize > MAX_DATAGRAM-16)
		return;

	sprintf(name,PATH_SPRITES"%s",texture);

	if(!org || !scale)
	{
		Console_ErrorMessage(FALSE,name,"Illegal parameters used for flare.");
		return;
	}

	MSG_WriteByte(&sv.datagram,SVC_FLARE);
	for(i=0;i<3;i++)
		MSG_WriteCoord(&sv.datagram,org[i]);
	MSG_WriteFloat(&sv.datagram,r);
	MSG_WriteFloat(&sv.datagram,g);
	MSG_WriteFloat(&sv.datagram,b);
	MSG_WriteFloat(&sv.datagram,a);
	MSG_WriteFloat(&sv.datagram,scale);
	for(i=0;i<MAX_EFFECTS;i++)
		if(gEffectTexture[i])
			if(!Q_strcmp(name,gEffectTexture[i]->name))
			{
				MSG_WriteByte(&sv.datagram,i);
				break;
			}
#endif
}

/*
	Particles
*/

void Particle(MathVector3f_t org, MathVector3f_t dir,float scale,char *texture,int count)
{
	int	i,v;

	if(sv.datagram.cursize > MAX_DATAGRAM-16)
		return;

	MSG_WriteByte(&sv.datagram,SVC_PARTICLE);

	for(i = 0; i < 3; i++)
		MSG_WriteCoord(&sv.datagram,org[i]);

	for(i=0 ; i<3 ; i++)
	{
		v = dir[i]*16;
		if (v > 127)
			v = 127;
		else if (v < -128)
			v = -128;

		MSG_WriteChar(&sv.datagram,v);
	}

	MSG_WriteFloat(&sv.datagram,scale);

	for(i = 0; i < MAX_EFFECTS; i++)
		if(gEffectTexture[i])
		{
			if(!strcmp(texture,gEffectTexture[i]->name))
			{
				MSG_WriteByte(&sv.datagram,i);
				break;
			}
		}
		else
		{
			// Otherwise give the texture the initial slot (this points to the notexture).
			MSG_WriteByte(&sv.datagram,0);
			break;
		}

	MSG_WriteByte(&sv.datagram,count);
}

/**/

void LightStyle(int style,char *val)
{
	client_t	*client;
	int			j;

	sv.lightstyles[style] = val;

	if (sv.state != SERVER_STATE_ACTIVE)
		return;

	for (j=0, client = svs.clients ; j<svs.maxclients ; j++, client++)
		if (client->active || client->bSpawned)
		{
			MSG_WriteChar (&client->message, svc_lightstyle);
			MSG_WriteChar (&client->message,style);
			MSG_WriteString (&client->message, val);
		}
}

ServerEntity_t	*eMessageEntity;

sizebuf_t *Game_MessageOne(ServerEntity_t *seMessage)
{
	int iEntity;

	iEntity = NUM_FOR_EDICT(seMessage);
	if (iEntity < 1 || iEntity > svs.maxclients)
		Con_Error("Attempted to send message to a non-client! (%i) (%s)", iEntity, seMessage->v.cClassname);

	return &svs.clients[iEntity - 1].message;
}

sizebuf_t *Game_WriteDest(int dest)
{
	switch(dest)
	{
	case MSG_BROADCAST:
		return &sv.datagram;
	case MSG_ONE:
		return Game_MessageOne(eMessageEntity);
	case MSG_ALL:
		return &sv.reliable_datagram;
	case MSG_INIT:
		return &sv.signon;
	default:
		Con_Error("WriteDest: bad destination");
		break;
	}

	return NULL;
}

void Game_WriteByte(int mode,int command)
{
	MSG_WriteByte(Game_WriteDest(mode),command);
}

void Game_WriteCoord(int mode,float f)
{
	MSG_WriteCoord(Game_WriteDest(mode),f);
}

void Game_WriteAngle(int mode,float f)
{
	MSG_WriteAngle(Game_WriteDest(mode),f);
}

void Game_WriteEntity(int mode, ServerEntity_t *ent)
{
	MSG_WriteShort(Game_WriteDest(mode),NUM_FOR_EDICT(ent));
}

void Game_AddCommand(char *c,void (*Function)(void))
{
	Cmd_AddCommand(c,(xcommand_t)Function);
}

void Game_SetMessageEntity(ServerEntity_t *eEntity)
{
	eMessageEntity = eEntity;
}

void Host_Restart_f(void);
void Server_CenterPrint(ServerEntity_t *ent, char *msg);	// See engine_server.

void Game_Initialize(void)
{
	bool			bGameLoaded = false;
	ModuleImport_t	Import;

	if(Game)
		pModule_Unload(hGameInstance);

	// Server
	Import.Con_Printf = Con_Printf;
	Import.Con_DPrintf = Con_DPrintf;
	Import.Con_Warning = Con_Warning;
	Import.Sys_Error = Sys_Error;
	Import.SetModel = Server_SetModel;
	Import.Particle = Particle;
	Import.Flare = Server_Flare;
	Import.Sound = Server_Sound;
	Import.UnlinkEntity = SV_UnlinkEdict;
	Import.LinkEntity = SV_LinkEdict;
	Import.Server_Move = SV_Move;
	Import.FreeEntity = ED_Free;
	Import.Spawn = ED_Alloc;
	Import.Cvar_RegisterVariable = Cvar_RegisterVariable;
	Import.Cvar_SetValue = Cvar_SetValue;
	Import.LightStyle = LightStyle;
	Import.CenterPrint = Server_CenterPrint;
	Import.Cmd_AddCommand = Game_AddCommand;
	Import.ReadByte = MSG_ReadByte;
	Import.ReadCoord = MSG_ReadCoord;
	Import.WriteByte = Game_WriteByte;
	Import.WriteCoord = Game_WriteCoord;
	Import.WriteAngle = Game_WriteAngle;
	Import.WriteEntity = Game_WriteEntity;
	Import.SetMessageEntity = Game_SetMessageEntity;

	// Hunk
	Import.Hunk_Alloc			= Hunk_Alloc;
	Import.Hunk_AllocateName	= Hunk_AllocName;

	// Client
	Import.Client_AllocateDlight = Client_AllocDlight;
	Import.Client_AllocateParticle = Particle_Allocate;
	Import.Client_PrecacheResource = Client_PrecacheResource;
	Import.Client_GetStat = Client_GetStat;
	Import.Client_GetEffect = Client_GetEffect;
	Import.Client_GetPlayerEntity = Client_GetPlayerEntity;
	Import.Client_GetViewEntity = Client_GetViewEntity;

	if(Menu)
	{
		Import.Client_AddMenuState		= Menu->AddState;
		Import.Client_RemoveMenuState	= Menu->RemoveState;
	}

	Import.Server_PointContents = SV_PointContents;
	Import.Server_MakeStatic = Server_MakeStatic;
	Import.Server_BroadcastPrint = SV_BroadcastPrintf;
	Import.Server_SinglePrint = Server_SinglePrint;
	Import.Server_PrecacheResource = Server_PrecacheResource;
	Import.Server_FindRadius = Server_FindRadius;
	Import.Server_FindEntity = Server_FindEntity;
	Import.Server_Restart = Host_Restart_f;
	Import.Server_ChangeLevel = Server_ChangeLevel;
	Import.Server_AmbientSound = Game_AmbientSound;
	Import.Server_GetLevelName = Server_GetLevelName;
	Import.Server_GetFrameTime = Server_GetFrameTime;
	Import.Server_GetNumEdicts = Game_GetNumEdicts;
	Import.Server_GetEdicts = Game_GetEdicts;

	Game = (GameExport_t*)pModule_LoadInterface(hGameInstance, va("%s/%s"MODULE_GAME, com_gamedir, Global.cModulePath), "Game_Main", &Import);
	if(!Game)
		Con_Warning(pError_Get(),com_gamedir,MODULE_GAME);
	else if (Game->iVersion != GAME_VERSION)
		Con_Warning("Size mismatch (recieved %i, expected %i)!\n", Game->iVersion, GAME_VERSION);
	else
		bGameLoaded = true;

	if(!bGameLoaded)
	{
		pModule_Unload(hGameInstance);

		// Let the user know the module failed to load.
		Sys_Error("Failed to load %s/%s."PLATFORM_CPU""pMODULE_EXTENSION"!\nCheck log for details.\n",com_gamedir,MODULE_GAME);
	}
}
