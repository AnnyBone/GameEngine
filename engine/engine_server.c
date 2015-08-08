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

#include "engine_server.h"

typedef struct
{
	const char		*cFieldName;

	int				iOffset;

	EntityType_t	eDataType;

	int				iFlags;
} EntityField_t;

EntityField_t	GlobalFields[] =
{
	{	"wad",			0,	EV_STRING		},
	{	"ambient",		0,	EV_NONE			},
	{	"sky",			0,	EV_STRING		},
	{	"cloud",		0,	EV_STRING		},
	{	"scrollspeed",	0,	EV_INTEGER		},

	{	0	}
};

EntityField_t	EntityFields[] =
{
	{ "classname", ENTITY_FIELD(v.cClassname), EV_STRING },
	{ "name", ENTITY_FIELD(v.cName), EV_STRING },
	{ "model", ENTITY_FIELD(v.model), EV_STRING },
	{ "targetname", ENTITY_FIELD(v.targetname), EV_STRING },
	{ "noise", ENTITY_FIELD(v.noise), EV_STRING },
	{ "message", ENTITY_FIELD(v.message), EV_STRING },
	{ "origin", ENTITY_FIELD(v.origin), EV_VECTOR },
	{ "angles", ENTITY_FIELD(v.angles), EV_VECTOR },
	{ "light", ENTITY_FIELD(v.vLight), EV_VECTOR4 },
	{ "health", ENTITY_FIELD(v.iHealth), EV_INTEGER },
	{ "spawnflags", ENTITY_FIELD(v.spawnflags), EV_INTEGER },
	{ "bTakeDamage", ENTITY_FIELD(v.bTakeDamage), EV_BOOLEAN },
	{ "takedamage", ENTITY_FIELD(v.bTakeDamage), EV_BOOLEAN },
	{ "alpha", ENTITY_FIELD(alpha), EV_INTEGER },

	// Model properties
	{ "model_skin", ENTITY_FIELD(Model.iSkin), EV_INTEGER },
	{ "scale", ENTITY_FIELD(Model.fScale), EV_FLOAT },

	// Physical properties
	{ "physics_solid", ENTITY_FIELD(Physics.iSolid), EV_INTEGER },
	{ "physics_mass", ENTITY_FIELD(Physics.fMass), EV_FLOAT },
	{ "physics_gravity", ENTITY_FIELD(Physics.fGravity), EV_FLOAT },

	// Local (move these at some point)
	{ "sound", ENTITY_FIELD(local.sound), EV_STRING },
	{ "soundstart", ENTITY_FIELD(local.cSoundStart), EV_STRING },
	{ "soundstop", ENTITY_FIELD(local.cSoundStop), EV_STRING },
	{ "soundmoving", ENTITY_FIELD(local.cSoundMoving), EV_STRING },
	{ "soundreturn", ENTITY_FIELD(local.cSoundReturn), EV_STRING },
	{ "target1", ENTITY_FIELD(local.cTarget1), EV_STRING },
	{ "target2", ENTITY_FIELD(local.cTarget2), EV_STRING },
	{ "speed", ENTITY_FIELD(local.speed), EV_FLOAT },
	{ "delay", ENTITY_FIELD(local.delay), EV_FLOAT },
	{ "lip", ENTITY_FIELD(local.lip), EV_FLOAT },
	{ "wait", ENTITY_FIELD(local.dWait), EV_DOUBLE },
	{ "damage", ENTITY_FIELD(local.iDamage), EV_INTEGER },
	{ "volume", ENTITY_FIELD(local.volume), EV_INTEGER },
	{ "style", ENTITY_FIELD(local.style), EV_INTEGER },
	{ "count", ENTITY_FIELD(local.count), EV_INTEGER },
	{ "pTeam", ENTITY_FIELD(local.pTeam), EV_INTEGER },
	{ "attack_finished", ENTITY_FIELD(local.dAttackFinished), EV_DOUBLE },

	// hacks
	{ "angle", ENTITY_FIELD(v.angles), EV_VECTOR, FL_ANGLEHACK },

	// Global fields to ignore... Ugh ~hogsy
	{ "wad", 0, EV_NONE },
	{ "ambient", 0, EV_NONE },
	{ "sky", 0, EV_NONE },
	{ "cloud", 0, EV_NONE },
	{ "scrollspeed", 0, EV_NONE },
	{ "mapversion", 0, EV_NONE },

	{	0	}
};

/*	Based on ED_ParseField from Quake 2.
	TODO:
		Better error handling?
*/
void Server_ParseEntityField(char *cKey, char *cValue, ServerEntity_t *eEntity)
{
	EntityField_t *eField;
	MathVector4f_t vVector;

	for(eField = EntityFields; eField->cFieldName; eField++)
	{
		if(!Q_strcasecmp((char*)eField->cFieldName,cKey))
		{
			switch(eField->eDataType)
			{
				case EV_STRING:
					*(char**)((uint8_t*)eEntity + eField->iOffset) = ED_NewString(cValue);
					break;
				case EV_VECTOR:
					switch(eField->iFlags)
					{
					case FL_ANGLEHACK:
						((float*)((uint8_t*)eEntity+eField->iOffset))[0] = 0;
						((float*)((uint8_t*)eEntity + eField->iOffset))[1] = strtof(cValue, NULL);
						((float*)((uint8_t*)eEntity + eField->iOffset))[2] = 0;
						break;
					default:
#ifdef _MSC_VER
#pragma warning(suppress: 6031)
#endif
						sscanf(cValue,"%f %f %f",&vVector[0],&vVector[1],&vVector[2]);
						((float*)((uint8_t*)eEntity + eField->iOffset))[0] = vVector[0];
						((float*)((uint8_t*)eEntity + eField->iOffset))[1] = vVector[1];
						((float*)((uint8_t*)eEntity + eField->iOffset))[2] = vVector[2];
					}
					break;
				case EV_VECTOR4:
#ifdef _MSC_VER
#pragma warning(suppress: 6031)
#endif
					sscanf(cValue,"%f %f %f %f",&vVector[0],&vVector[1],&vVector[2],&vVector[3]);
					((float*)((uint8_t*)eEntity + eField->iOffset))[0] = vVector[0];
					((float*)((uint8_t*)eEntity + eField->iOffset))[1] = vVector[1];
					((float*)((uint8_t*)eEntity + eField->iOffset))[2] = vVector[2];
					((float*)((uint8_t*)eEntity + eField->iOffset))[3] = vVector[3];
					break;
				case EV_FLOAT:
					*(float*)((uint8_t*)eEntity + eField->iOffset) = strtof(cValue, NULL);
					break;
				case EV_DOUBLE:
					*(double*)((uint8_t*)eEntity + eField->iOffset) = strtod(cValue, NULL);
					break;
				case EV_BOOLEAN:
					if(!Q_strcasecmp(cValue,"true"))
						cValue = "1";
					else if(!Q_strcasecmp(cValue,"false"))
						cValue = "0";
					// [2/1/2013] Booleans are handled in the same way as integers, so don't break here! ~hogsy
				case EV_INTEGER:
					*(int*)((uint8_t*)eEntity+eField->iOffset) = atoi(cValue);
					break;
				// [22/11/2012] Just ignore anything that has skip set! ~hogsy
				case EV_NONE:
					break;
				default:
					Con_Warning("Unknown entity field type! (%i)\n",(int)eField->eDataType);
			}
			return;
		}
	}

	Con_Warning("Invalid field! (%s)\n",cKey);
}

void Server_ParseGlobalField(char *cKey,char *cValue)
{
	EntityField_t	*eField;

	for(eField = GlobalFields; eField->cFieldName; eField++)
	{
		if(!Q_strcasecmp((char*)eField->cFieldName,cKey))
		{
#if 0	// todo
			switch(eField->eDataType)
			{
				case EV_STRING:
					*(char**)((byte*)eEntity+eField->iOffset) = ED_NewString(cValue);
					break;
				case EV_VECTOR:
					switch(eField->iFlags)
					{
					case FL_ANGLEHACK:
						((float*)((byte*)eEntity+eField->iOffset))[0] = 0;
						((float*)((byte*)eEntity+eField->iOffset))[1] = strtof(cValue,NULL);
						((float*)((byte*)eEntity+eField->iOffset))[2] = 0;
						break;
					default:
						sscanf(cValue,"%f %f %f",&vVector[0],&vVector[1],&vVector[2]);
						((float*)((byte*)eEntity+eField->iOffset))[0] = vVector[0];
						((float*)((byte*)eEntity+eField->iOffset))[1] = vVector[1];
						((float*)((byte*)eEntity+eField->iOffset))[2] = vVector[2];
					}
					break;
				case EV_VECTOR4:
					sscanf(cValue,"%f %f %f %f",&vVector[0],&vVector[1],&vVector[2],&vVector[3]);
					((float*)((byte*)eEntity+eField->iOffset))[0] = vVector[0];
					((float*)((byte*)eEntity+eField->iOffset))[1] = vVector[1];
					((float*)((byte*)eEntity+eField->iOffset))[2] = vVector[2];
					((float*)((byte*)eEntity+eField->iOffset))[3] = vVector[3];
					break;
				case EV_FLOAT:
					*(float*)((byte*)eEntity+eField->iOffset) = strtof(cValue,NULL);
					break;
				case EV_DOUBLE:
					*(double*)((byte*)eEntity+eField->iOffset) = strtod(cValue,NULL);
					break;
				case EV_BOOLEAN:
					if(!Q_strcasecmp(cValue,"true"))
						cValue = "1";
					else if(!Q_strcasecmp(cValue,"false"))
						cValue = "0";
					// [2/1/2013] Booleans are handled in the same way as integers, so don't break here! ~hogsy
				case EV_INTEGER:
					*(int*)((byte*)eEntity+eField->iOffset) = atoi(cValue);
					break;
				// [22/11/2012] Just ignore anything that has skip set! ~hogsy
				case EV_NONE:
					break;
				default:
					Con_Warning("Unknown entity field type! (%i)\n",(int)eField->eDataType);
			}
			return;
#endif
		}
	}

	Con_Warning("Invalid field! (%s)\n",cKey);
}

ServerEntity_t *Server_FindEntity(ServerEntity_t *eStartEntity, char *cName, bool bClassname)
{
	int	i;
	ServerEntity_t *eEntity = eStartEntity;

	// Ensure that the first entity is valid.
	if (!eStartEntity)
	{
		Con_Warning("Attempted to start search from an invalid entity! (%s)", cName);
		return NULL;
	}

	for(i = 0; i < sv.num_edicts; i++,eEntity = NEXT_EDICT(eEntity))
	{
		if(eEntity->free)
			continue;

		if(bClassname)
		{
			if(eEntity->v.cClassname)
				if(!Q_strcmp(cName,eEntity->v.cClassname))
					return eEntity;
		}
		else
		{
			if(eEntity->v.cName)
				if(!Q_strcmp(cName,eEntity->v.cName))
					return eEntity;
		}
	}

	return NULL;
}

// [9/7/2012] Added Server_PrecacheResource ~hogsy
void Server_PrecacheResource(int iType,const char *ccResource)
{
	char	name[MAX_OSPATH];
	int		i;

	/*	[13/7/2012] TODO:
			Set extension automatically based on type.

		~hogsy
	*/

	if(sv.state != SERVER_STATE_LOADING)
	{
		Con_Warning("Server attempted to precache resource during runtime! (%s)\n",ccResource);
		return;
	}

	switch(iType)
	{
	case RESOURCE_MODEL:
		for (i = 0; i < MAX_MODELS; i++)
			if(!sv.model_precache[i])
			{
				sv.model_precache[i]	= (char*)ccResource;
				sv.models[i]			= Mod_ForName((char*)ccResource);
				return;
			}
			else if(!strcmp(sv.model_precache[i],ccResource))
				return;

		Console_ErrorMessage(false,ccResource,"Overflow!");
		break;
	case RESOURCE_SPRITE:
		sprintf(name,PATH_SPRITES"%s",ccResource);
		for(i = 0; i < MAX_PARTICLES; i++)
			if(!sv.cParticlePrecache[i])
			{
				sv.cParticlePrecache[i]	= (char*)ccResource;
				return;
			}
			else if(!strcmp(name,sv.cParticlePrecache[i]))
				return;

		Console_ErrorMessage(false,ccResource,"Overflow!");
		break;
	case RESOURCE_SOUND:
		for(i = 0; i < MAX_SOUNDS; i++)
			if(!sv.sound_precache[i])
			{
				sv.sound_precache[i] = (char*)ccResource;
				return;
			}
			else if(!strcmp(sv.sound_precache[i],ccResource))
				return;

		Console_ErrorMessage(false,(char*)ccResource,"Overflow!");
		break;
	default:
		Con_Warning("Attempted to precache an undefined type! (%s) (%i)\n",ccResource,iType);
	}
}

/*
	Console Messages
*/

void Server_SinglePrint(ServerEntity_t *eEntity, char *cMessage)
{
	client_t	*cClient;
	int			iEntity	= NUM_FOR_EDICT(eEntity);

	if(iEntity > svs.maxclients)
	{
		Con_Warning("Attempted to send message to a non-client! (%s)\n",cMessage);
		return;
	}

	cClient = &svs.clients[iEntity-1];

	MSG_WriteChar(&cClient->message,SVC_PRINT);
	MSG_WriteString(&cClient->message,cMessage);
}

// [25/2/2012] Added CenterPrint ~hogsy
// [18/7/2012] Renamed to Server_CenterPrint ~hogsy
void Server_CenterPrint(ServerEntity_t *ent, char *msg)
{
	client_t	*client;
	int			entnum = NUM_FOR_EDICT(ent);

	if(entnum < 1 || entnum > svs.maxclients)
	{
		Con_Warning("Attempted to send message to a non-client! (%s)\n",msg);
		return;
	}

	client = &svs.clients[entnum-1];

	MSG_WriteChar(&client->message,svc_centerprint);
	MSG_WriteString(&client->message,msg);
}

/*
	Utilities
*/

double Server_GetFrameTime(void)
{
	return host_frametime;
}
