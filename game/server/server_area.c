/*	Copyright (C) 2011-2016 OldTimes Software

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

#include "server_main.h"

/*
	Brush-based entities that span across an area.
	This document contains the code for the following
	entities and its sub-methods:

	area_breakable 		- Brush that breaks and spawns debris
	area_rotate		- Rotates around specified axis
	area_door		- Opens and closes in specified position
	area_changelevel	- Changes the current level
	area_trigger		- Triggers a point entity
	area_pushable		- Brush that can be pushed around
	area_wall		- Brush that casts no shadow and can appear/disappear on use
	area_button		- Trigger a point entity when moved into place
	area_platform		- Platform that travels between two destinations
	area_climb		- Substitute for a ladder
	area_noclip		- Brush that casts no light and no collision
	area_push		- Pushes entities into the specified direction on touch

TODO:
	area_monsterclip ?	- Blocks ways off for monsters
*/

void Area_SetMoveDirection(MathVector3f_t vAngles, MathVector3f_t vMoveDirection)
{
	MathVector3f_t vUp = { 0, -1, 0 };
	MathVector3f_t vMoveUp = { 0, 0, 1 };
	MathVector3f_t vDown = { 0, -2, 0 };
	MathVector3f_t vMoveDown = { 0, 0, -1 };

	if(Math_VectorCompare(vAngles,vUp))
		Math_VectorCopy(vMoveUp,vMoveDirection);
	else if(Math_VectorCompare(vAngles,vDown))
		Math_VectorCopy(vMoveDown,vMoveDirection);
	else
		Math_AngleVectors(vAngles,vMoveDirection,NULL,NULL);

	Math_VectorClear(vAngles);
}

void Area_CalculateMovementDone(ServerEntity_t *eArea)
{
	Entity_SetOrigin(eArea,eArea->local.finaldest);

	Math_VectorClear(eArea->v.velocity);

	if(eArea->local.think1)
		eArea->local.think1(eArea,eArea);
}

void Area_CalculateMovement(ServerEntity_t *eArea, MathVector3f_t vTDest, float fSpeed, void(*Function)(ServerEntity_t *eArea, ServerEntity_t *eOther))
{
	MathVector3f_t	vdestdelta;
	float	fTravelTime;

	Math_VectorCopy(vTDest,eArea->local.finaldest);

	Math_VectorSubtract(vTDest,eArea->v.origin,vdestdelta);

	fTravelTime = (float)Math_VectorLength(vdestdelta)/fSpeed;

	Math_VectorScale(vdestdelta,1.0f/fTravelTime,eArea->v.velocity);

	eArea->local.think1	= Function;

	eArea->v.think		= Area_CalculateMovementDone;
	eArea->v.dNextThink	= eArea->v.ltime+fTravelTime;
}

/*
	Spawn
*/

/*	Area in which players can be randomly spawned.
	TODO: Finish this!
*/
void Area_PlayerSpawn(ServerEntity_t *eArea)
{
	//Waypoint_Spawn(eArea->v.origin,WAYPOINT_SPAWNAREA);
}

/*
	Breakable
*/

#define BREAKABLE_GLASS	0
#define	BREAKABLE_WOOD	1
#define	BREAKABLE_ROCK	2
#define	BREAKABLE_METAL	3

void Area_BreakableBounce(ServerEntity_t *eGib, ServerEntity_t *eOther)
{
	char cSound[128];

	if(eGib->v.flags & FL_ONGROUND)
		return;

	switch(eGib->local.style)
	{
	case BREAKABLE_GLASS:
		PHYSICS_SOUND_GLASS(cSound);
		break;
	case BREAKABLE_WOOD:
		PHYSICS_SOUND_WOOD(cSound);
		break;
	case BREAKABLE_ROCK:
		PHYSICS_SOUND_ROCK(cSound);
		break;
	case BREAKABLE_METAL:
		PHYSICS_SOUND_METAL(cSound);
		break;
	default:
		Engine.Con_Warning("Unknown breakable type! (%i)\n",eGib->local.style);
		return;
	}

	Sound(eGib,CHAN_AUTO,cSound,30,ATTN_NORM);
}

void Area_CreateGib(ServerEntity_t *eArea, const char *cModel)
{
	int	j;
	ServerEntity_t *eGib;

	eGib = Entity_Spawn();
	if (eGib)
	{
		eGib->v.cClassname = "entity_gib";
		eGib->v.movetype = MOVETYPE_BOUNCE;
		eGib->v.TouchFunction = Area_BreakableBounce;
		eGib->v.think = Entity_Remove;
		eGib->v.dNextThink = Server.dTime + 20;
		eGib->v.bTakeDamage = false;

		eGib->Physics.iSolid = SOLID_TRIGGER;

		eGib->local.style = eArea->local.style;

		for (j = 0; j < 3; j++)
		{
			eGib->v.velocity[j] =
				eGib->v.avelocity[j] = (float)(rand() % 5 * eArea->v.iHealth * 5);
		}

		Entity_SetModel(eGib, (char*)cModel);
		Entity_SetOrigin(eGib, eArea->v.oldorigin);
		Entity_SetSizeVector(eGib, g_mvOrigin3f, g_mvOrigin3f);
	}
}

void Area_BreakableDie(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	int	i;
	char cSound[128], cModel[PLATFORM_MAX_PATH];

	switch (eArea->local.style)
	{
	case BREAKABLE_GLASS:
		PHYSICS_SOUND_GLASS(cSound);
		PHYSICS_MODEL_GLASS(cModel);
		break;
	case BREAKABLE_WOOD:
		PHYSICS_SOUND_WOOD(cSound);
		PHYSICS_MODEL_WOOD(cModel);
		break;
	case BREAKABLE_ROCK:
		PHYSICS_SOUND_ROCK(cSound);
		PHYSICS_MODEL_ROCK(cModel);
		break;
	case BREAKABLE_METAL:
		PHYSICS_SOUND_METAL(cSound);
		PHYSICS_MODEL_METAL(cModel);
		break;
	}

	Sound(eArea, CHAN_AUTO, cSound, 255, ATTN_STATIC);

	for (i = 0; i < eArea->local.count; i++)
		Area_CreateGib(eArea, cModel);	

	// Needs to be set to prevent a recursion.
	eArea->v.bTakeDamage = false;

	if (eArea->v.targetname) // Trigger doors, etc. ~eukos
		UseTargets(eArea, eOther);

	Entity_Remove(eArea);
}

void Area_BreakableUse(ServerEntity_t *eArea)
{
	Area_BreakableDie(eArea, eArea->local.activator);
}

void Area_BreakableSpawn(ServerEntity_t *eArea)
{
	if (eArea->v.iHealth <= 0)
		eArea->v.iHealth = 1;

	switch (eArea->local.style)
	{
	case BREAKABLE_GLASS:
		Server_PrecacheSound( PHYSICS_SOUND_GLASS0);
		Server_PrecacheSound( PHYSICS_SOUND_GLASS1);
		Server_PrecacheSound( PHYSICS_SOUND_GLASS2);
		Server_PrecacheModel(PHYSICS_MODEL_GLASS0);
		Server_PrecacheModel(PHYSICS_MODEL_GLASS1);
		Server_PrecacheModel(PHYSICS_MODEL_GLASS2);
		break;
	case BREAKABLE_WOOD:
		Server_PrecacheSound( PHYSICS_SOUND_WOOD0);
		Server_PrecacheSound( PHYSICS_SOUND_WOOD1);
		Server_PrecacheSound( PHYSICS_SOUND_WOOD2);
		Server_PrecacheModel(PHYSICS_MODEL_WOOD0);
		Server_PrecacheModel(PHYSICS_MODEL_WOOD1);
		Server_PrecacheModel(PHYSICS_MODEL_WOOD2);
		break;
	case BREAKABLE_ROCK:
		Server_PrecacheSound( PHYSICS_SOUND_ROCK0);
		Server_PrecacheSound( PHYSICS_SOUND_ROCK1);
		Server_PrecacheSound( PHYSICS_SOUND_ROCK2);
		Server_PrecacheModel(PHYSICS_MODEL_ROCK0);
		Server_PrecacheModel(PHYSICS_MODEL_ROCK1);
		Server_PrecacheModel(PHYSICS_MODEL_ROCK2);
		break;
	case BREAKABLE_METAL:
		Server_PrecacheSound( PHYSICS_SOUND_METAL0);
		Server_PrecacheSound( PHYSICS_SOUND_METAL1);
		Server_PrecacheSound( PHYSICS_SOUND_METAL2);
		Server_PrecacheModel(PHYSICS_MODEL_METAL0);
		Server_PrecacheModel(PHYSICS_MODEL_METAL1);
		Server_PrecacheModel(PHYSICS_MODEL_METAL2);
		break;
	default:
		Engine.Con_Warning("area_breakable: Unknown style\n");
	}

	// If we've been given a name, then set our use function.
	if (eArea->v.cName)
		eArea->v.use = Area_BreakableUse;

	eArea->Physics.iSolid = SOLID_BSP;

	eArea->v.movetype = MOVETYPE_PUSH;
	eArea->v.bTakeDamage = true;

	eArea->local.bBleed = false;

	Entity_SetKilledFunction(eArea, Area_BreakableDie);

	Entity_SetModel(eArea, eArea->v.model);
	Entity_SetOrigin(eArea, eArea->v.origin);
	Entity_SetSizeVector(eArea, eArea->v.mins, eArea->v.maxs);

	eArea->v.oldorigin[0] = (eArea->v.mins[0] + eArea->v.maxs[0])*0.5f;
	eArea->v.oldorigin[1] = (eArea->v.mins[1] + eArea->v.maxs[1])*0.5f;
	eArea->v.oldorigin[2] = (eArea->v.mins[2] + eArea->v.maxs[2])*0.5f;
}

/**/

void Area_RotateBlocked(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	Entity_Damage(eOther, eArea, eArea->local.iDamage, DAMAGE_TYPE_CRUSH);
}

void Area_RotateTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	if(eArea->local.dMoveFinished > Server.dTime)
		return;
}

void Area_RotateThink(ServerEntity_t *eArea)
{
	eArea->v.dNextThink	= Server.dTime+1000000000.0;
}

#define STYLE_ROTATE_DOOR	1

#define	SPAWNFLAG_ROTATE_X			2
#define	SPAWNFLAG_ROTATE_Y			4
#define	SPAWNFLAG_ROTATE_Z			8
#define	SPAWNFLAG_ROTATE_REVERSE	64

void Area_RotateSpawn(ServerEntity_t *eArea)
{
	if(!eArea->local.speed)
		eArea->local.speed = 100.0f;

#if 0
	// [26/7/2012] Check our spawn flags ~hogsy
	if(eArea->local.style == STYLE_ROTATE_DOOR)
	{
		if(eArea->v.spawnflags & SPAWNFLAG_ROTATE_REVERSE)
		{
		}

		if(eArea->v.spawnflags & SPAWNFLAG_ROTATE_X)
			eArea->v.movedir[0] = 1.0f;
		else if(eArea->v.spawnflags & SPAWNFLAG_ROTATE_Y)
			eArea->v.movedir[1] = 1.0f;
		else
			eArea->v.movedir[2] = 1.0f;

		Math_VectorCopy(eArea->v.angles,eArea->local.pos1);
		eArea->local.pos2[0] = eArea->local.pos1[0]+eArea->v.movedir[0]*eArea->local.distance;

		eArea->v.TouchFunction = Area_RotateTouch;

		eArea->local.dMoveFinished = 0;
	}
	else
#endif
	{
		if(eArea->v.spawnflags & SPAWNFLAG_ROTATE_REVERSE)
			eArea->local.speed *= -1;

		if(eArea->v.spawnflags & SPAWNFLAG_ROTATE_X)
			eArea->v.avelocity[0] = eArea->local.speed;

		if(eArea->v.spawnflags & SPAWNFLAG_ROTATE_Y)
			eArea->v.avelocity[1] = eArea->local.speed;

		if(eArea->v.spawnflags & SPAWNFLAG_ROTATE_Z)
			eArea->v.avelocity[2] = eArea->local.speed;
	}

	Entity_SetBlockedFunction(eArea, Area_RotateBlocked);

	eArea->v.movetype	= MOVETYPE_PUSH;
	eArea->v.think		= Area_RotateThink;
	eArea->v.dNextThink	= Server.dTime+1000000000.0;

	eArea->Physics.iSolid = SOLID_BSP;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetSizeVector(eArea,eArea->v.mins,eArea->v.maxs);
	Entity_SetOrigin(eArea,eArea->v.origin);
}

/*
	AREA_TRIGGERFIELD
*/

ServerEntity_t *Area_SpawnTriggerField(ServerEntity_t *owner, MathVector3f_t mins, MathVector3f_t maxs, void (*TriggerFunction)(ServerEntity_t *entity, ServerEntity_t *other))
{
	if (Math_VectorCompare(g_mvOrigin3f, mins) ||
		Math_VectorCompare(g_mvOrigin3f, maxs))
	{
		g_engine->Con_Warning("Invalid size for trigger field!");
		return NULL;
	}

	ServerEntity_t *field = Entity_Spawn();
	field->v.movetype		= MOVETYPE_NONE;
	field->local.eOwner		= owner;

	Entity_SetTouchFunction(field, TriggerFunction);
	Entity_SetPhysics(field, SOLID_TRIGGER, 0, 0);
	
	// Set the size of it.
	MathVector3f_t tmins, tmaxs;
	Math_VectorCopy(mins, tmins);
	Math_VectorCopy(maxs, tmaxs);
	tmins[0] -= 60;	tmins[1] -= 60;	tmins[2] -= 8;
	tmaxs[0] += 60;	tmaxs[1] += 60;	tmaxs[2] += 8;
	Entity_SetSizeVector(field, tmins, tmaxs);

	return field;
}

/*
	AREA_DOOR
*/

// Spawn flags.
#define	DOOR_FLAG_TRIGGERTOUCH		1	// Triggered by touch.
#define	DOOR_FLAG_TRIGGERUSE		2	// Triggered by use command.
#define	DOOR_FLAG_TRIGGERAUTO		4	// Automatically open door on approach.
#define	DOOR_FLAG_TRIGGERDAMAGE		8	// Triggered by damage.
#define	DOOR_FLAG_LINK				16	// Link the door with others in prox.

// Various states for doors.
#define STATE_TOP			0	// Brush is at the top.
#define STATE_BOTTOM		1	// Brush is at the bottom.
#define STATE_UP			2	// Brush is moving up.
#define STATE_DOWN			3	// Brush is moving down.

void Area_DoorDone(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	if (eArea->local.sound_stop[0] != ' ')
		Sound(eArea, CHAN_VOICE, eArea->local.sound_stop, 255, ATTN_NORM);
}

void Area_DoorReturn(ServerEntity_t *eArea)
{
	eArea->local.iLocalFlags = STATE_DOWN;

	Area_CalculateMovement(eArea, eArea->local.pos1, eArea->local.speed, Area_DoorDone);

	if(eArea->local.cSoundReturn[0] != ' ')
		Sound(eArea,CHAN_BODY,eArea->local.cSoundReturn,255,ATTN_NORM);
	if(eArea->local.cSoundMoving[0] != ' ')
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_DoorWait(ServerEntity_t *door, ServerEntity_t *eOther)
{
	UseTargets(door, eOther);

	door->local.iLocalFlags = STATE_TOP;

	if (door->local.dWait >= 0)
		door->v.dNextThink = Server.dTime + door->local.dWait;

	door->v.think = Area_DoorReturn;

	if (door->local.sound_stop[0] != ' ')
		Sound(door, CHAN_VOICE, door->local.sound_stop, 255, ATTN_NORM);
}

void Area_DoorUse(ServerEntity_t *eArea)
{
	if(eArea->local.iLocalFlags == STATE_UP || eArea->local.iLocalFlags == STATE_TOP)
		return;

	eArea->local.iLocalFlags = STATE_UP;

	Area_CalculateMovement(eArea,eArea->local.pos2,eArea->local.speed,Area_DoorWait);

	if(eArea->local.cSoundStart)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundStart,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_DoorTouch(ServerEntity_t *door, ServerEntity_t *other)
{
	if (door->local.iLocalFlags == STATE_UP || door->local.iLocalFlags == STATE_TOP)
		return;
	if ((other->Monster.iType != MONSTER_PLAYER) && other->v.iHealth <= 0)
		return;

	// Door is linked!
	if (door->v.spawnflags & DOOR_FLAG_LINK)
	{
		ServerEntity_t *linked = door->local.activator;
		if (linked)
		{
			do
			{
				Area_DoorUse(linked);
				linked = linked->local.activator;
			} while (linked);
		}
		else
			g_engine->Con_Warning("No doors linked! (%i %i %i)\n",
			(int)door->v.origin[0],
			(int)door->v.origin[1],
			(int)door->v.origin[2]);
	}

	door->local.iLocalFlags = STATE_UP;
	Area_CalculateMovement(door, door->local.pos2, door->local.speed, Area_DoorWait);

	if (door->local.cSoundStart)
		Sound(door, CHAN_BODY, door->local.cSoundStart, 255, ATTN_NORM);
	if (door->local.cSoundMoving)
		Sound(door, CHAN_VOICE, door->local.cSoundMoving, 255, ATTN_NORM);
}

void Area_DoorBlocked(ServerEntity_t *door, ServerEntity_t *other)
{
	Entity_Damage(other, door, door->local.iDamage, DAMAGE_TYPE_CRUSH);
}

/*	Links doors together into a list.

	This is based on the original QC code; 
	https://github.com/maikmerten/zernichter/blob/master/id1/src/doors.qc#L316
*/
void Area_DoorLink(ServerEntity_t *door)
{
	ServerEntity_t	*prev;

	// Don't link if we've already done so.
	if (door->local.activator)
		return;

	// TODO: calculate overall scale of door?
	ServerEntity_t *link = g_engine->Server_FindRadius(door->v.origin, 700.0f);
	if (!link)
		return;

	// We're the master, yo.
	door->local.activator	= door;
	prev					= door;

	MathVector3f_t	smaxs, smins;
	Math_VectorSet(0, smins);
	Math_VectorSet(0, smaxs);

	// Copy the size over, which we'll use for the trigger field.
	plVectorCopy3f(door->v.mins, smins);
	plVectorCopy3f(door->v.maxs, smaxs);

	do
	{
		// Only link doors which will support it!
		if (!(link->v.spawnflags & DOOR_FLAG_LINK))
			// Skip to the next one.
			goto SKIP;

		if (Entity_IsTouching(prev, link))
		{
			if (link->local.activator)
			{
				g_engine->Con_Warning("Door already has activator assigned! (%i %i %i)\n", 
					(int)link->v.origin[0], (int)link->v.origin[1], (int)link->v.origin[2]);
				goto SKIP;
			}

			link->local.activator	= prev;
			prev					= link;

			// Update the bounds, which we'll use for the trigger field.
			if (link->v.mins[0] < smins[0])
				smins[0] = link->v.mins[0];
			if (link->v.mins[1] < smins[1])
				smins[1] = link->v.mins[1];
			if (link->v.mins[2] < smins[2])
				smins[2] = link->v.mins[2];
			if (link->v.maxs[0] > smaxs[0])
				smaxs[0] = link->v.maxs[0];
			if (link->v.maxs[1] > smaxs[1])
				smaxs[1] = link->v.maxs[1];
			if (link->v.maxs[2] > smaxs[2])
				smaxs[2] = link->v.maxs[2];
		}

SKIP:
		link = link->v.chain;
	} while (link);

	door->local.trigger_field = Area_SpawnTriggerField(door, smins, smaxs, Area_DoorTouch);
}

void Area_DoorSpawn(ServerEntity_t *door)
{
	int				i;
	float			movedist;
	MathVector3f_t	movedir;

	if (door->local.cSoundStart)
		Server_PrecacheSound(door->local.cSoundStart);
	if (door->local.sound_stop)
		Server_PrecacheSound(door->local.sound_stop);
	if (door->local.cSoundMoving)
		Server_PrecacheSound(door->local.cSoundMoving);
	if (door->local.cSoundReturn)
		Server_PrecacheSound(door->local.cSoundReturn);

	door->v.movetype = MOVETYPE_PUSH;

	Entity_SetPhysics(door, SOLID_BSP, 1.0f, 1.0f);
	Entity_SetModel(door, door->v.model);
	Entity_SetOrigin(door, door->v.origin);
	Entity_SetSizeVector(door, door->v.mins, door->v.maxs);

	if (door->local.lip == 0)
		door->local.lip = 4.0f;

	Math_VectorCopy(door->v.origin, door->local.pos1);

	Area_SetMoveDirection(door->v.angles, door->v.movedir);

	for(i = 0; i < 3; i++)
		movedir[i] = (float)fabs(door->v.movedir[i]);

	movedist =
		movedir[0] * door->v.size[0] + 
		movedir[1] * door->v.size[1] + 
		movedir[2] * door->v.size[2] - 
		door->local.lip;

	Math_VectorMake(door->local.pos1, movedist, door->v.movedir, door->local.pos2);

	door->local.iLocalFlags = STATE_BOTTOM;

	// Set the spawn flags up.
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERUSE)
		door->v.use = Area_DoorUse;
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERTOUCH)
		door->v.TouchFunction = Area_DoorTouch;
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERAUTO)
	{

	}
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERDAMAGE)
	{
		// Give it at least one HP.
		if (!door->v.iHealth)
			door->v.iHealth = 1;

		door->v.bTakeDamage = true;
		Entity_SetDamagedFunction(door, Area_DoorTouch);
	}

	if (door->local.iDamage)
		Entity_SetBlockedFunction(door, Area_DoorBlocked);
}

/*
	Change Level
*/

void Area_ChangeLevelTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	// [2/1/2013] TODO: If coop wait for eOther players? ~hogsy

	if(!Entity_IsPlayer(eOther))
		return;

#if 0
	// [2/1/2013] Because we don't want to trigger it multiple times within the delay!!! ~hogsy
	eArea->v.solid		= SOLID_NOT;
	eArea->v.think		= Area_ChangelevelStart;
	eArea->v.dNextThink	= Server.dTime+eArea->local.delay;
#else
	eArea->Physics.iSolid		= SOLID_NOT;

	// [2/1/2013] Change the level! ~hogsy
	Engine.Server_ChangeLevel(eArea->v.targetname);
#endif
}

void Area_ChangeLevel(ServerEntity_t *eArea)
{
	if(!eArea->v.targetname)
	{
		Engine.Con_Warning("No targetname set for area_changelevel! (%i %i %i)\n",
			(int)eArea->v.origin[0],
			(int)eArea->v.origin[1],
			(int)eArea->v.origin[2]);
		return;
	}

	eArea->v.TouchFunction	= Area_ChangeLevelTouch;

	eArea->Physics.iSolid	= SOLID_TRIGGER;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);

	eArea->v.model = 0;
}

/*
	Trigger
*/

void Area_TriggerTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	if(eArea->v.dNextThink || ((eArea->Monster.iType != MONSTER_PLAYER) && eOther->v.iHealth <= 0))
		return;

	eArea->v.dNextThink = Server.dTime+eArea->local.dWait;

	UseTargets(eArea,eOther);

	if(eArea->local.dWait < 0)
		Entity_Remove(eArea);
}

void Area_TriggerSpawn(ServerEntity_t *eArea)
{
	if(!eArea->v.targetname)
	{
		Engine.Con_Warning("'targetname' not set for trigger! (%i %i %i)\n",
			(int)eArea->v.origin[0],
			(int)eArea->v.origin[1],
			(int)eArea->v.origin[2]);
		return;
	}

	eArea->v.TouchFunction = Area_TriggerTouch;

	eArea->Physics.iSolid = SOLID_TRIGGER;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);

	eArea->v.model = 0;
}

/*
	Pushable
*/

float IsOnTopOf (ServerEntity_t *eTop, ServerEntity_t *eBottom)
{
	if (eTop->v.absmin[2] < eBottom->v.absmax[2] - 3)
		return 0;
	if (eTop->v.absmin[2] > eBottom->v.absmax[2] + 2)
		return 0;
	if (eTop->v.absmin[1] > eBottom->v.absmax[1])
		return 0;
	if (eTop->v.absmax[1] < eBottom->v.absmin[1])
		return 0;
	if (eTop->v.absmin[0] > eBottom->v.absmax[0])
		return 0;
	if (eTop->v.absmax[0] < eBottom->v.absmin[0])
		return 0;

	return 1;
}

void Area_PushableThink(ServerEntity_t *eArea)
{
	// TODO: sort the physics stuff out engine side perhaps? BSP physics are hard.
	// only update when something happens ~eukara
	if((eArea->v.velocity[0] != 0) || (eArea->v.velocity[1] != 0) || (eArea->v.velocity[2] != 0))
	{
		Math_VectorScale(eArea->v.velocity, 0.9f, eArea->v.velocity); // slowly slow it down.
		eArea->v.velocity[2] = -300;	// fake physics... FIXME
		eArea->v.dNextThink = eArea->v.ltime + 0.5f;
	}
}

void Area_PushableTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	float fYaw;
	MathVector3f_t vMVec;
	MathVector3f_t vPVec;

	if (IsOnTopOf(eOther, eArea)) 
	{
		// So the player can jump off the object.
		eOther->v.flags = eOther->v.flags + FL_ONGROUND; 
		return;
	}
	
	if (eArea->v.flags & FL_ONGROUND)
		eArea->v.flags = eArea->v.flags - FL_ONGROUND;
	
	// Get the right player angle.
	Math_MVToVector(Math_VectorToAngles(eOther->v.velocity), vPVec); 
	fYaw = vPVec[1] * ((float)pMath_PI) * 2 / 360;
	
	vMVec[0] = cosf(fYaw) * 80.0f;
	vMVec[1] = sinf(fYaw) * 80.0f;
	// Don't affect the height when pushing... ever.
	vMVec[2] = 0; 

	Math_VectorCopy(vMVec, eArea->v.velocity);
	Math_MVToVector(Math_VectorToAngles(eArea->v.velocity), eArea->v.avelocity);
	eArea->v.dNextThink = eArea->v.ltime + 0.5f;
}

void Area_PushableSpawn(ServerEntity_t *eArea)
{
	// TODO: If designed to be breakable, make breakable? ~eukara
/*	if (eArea->v.health) {
		Area_BreakableSpawn(eArea);
	}
*/
	//eArea->Physics.fGravity = cvServerGravity.value;
	Math_VectorClear(eArea->v.angles);

	eArea->Physics.iSolid = SOLID_SLIDEBOX;
	eArea->v.movetype = MOVETYPE_STEP;
	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
	Entity_SetSizeVector(eArea,eArea->v.mins,eArea->v.maxs);

	eArea->v.TouchFunction = Area_PushableTouch;
	eArea->v.think = Area_PushableThink;
}

/*
	Wall
*/

void Area_WallUse(ServerEntity_t *eArea)
{
	if(eArea->Physics.iSolid == SOLID_BSP)
	{
		eArea->local.cOldModel	= eArea->v.model;
		eArea->local.iValue		= 0;

		eArea->Physics.iSolid = SOLID_NOT;

		eArea->v.model = 0;
		return;
	}

	eArea->Physics.iSolid = SOLID_BSP;
	eArea->local.iValue = 1;
	Entity_SetModel(eArea,eArea->local.cOldModel);
}

void Area_WallSpawn(ServerEntity_t *eArea)
{
	if(!eArea->v.model)
	{
		Engine.Con_Warning("Area entity with no model!\n");

		Entity_Remove(eArea);
		return;
	}

	if(eArea->v.cName)
		eArea->v.use = Area_WallUse;

	eArea->v.movetype = MOVETYPE_PUSH;

	eArea->local.iValue = 1;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
}

/*
	Detail
*/
// Just for the compiler ~eukara
void Area_DetailSpawn(ServerEntity_t *eArea)
{
	if(!eArea->v.model)
	{
		Engine.Con_Warning("Area entity with no model!\n");

		Entity_Remove(eArea);
		return;
	}

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
}

/*
	Button
*/

void Area_ButtonDone(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	eArea->local.iLocalFlags = STATE_DOWN;
	eArea->local.iValue = 0;

	if (eArea->local.sound_stop)
		Sound(eArea, CHAN_VOICE, eArea->local.sound_stop, 255, ATTN_NORM);
}

void Area_ButtonReturn(ServerEntity_t *eArea)
{
	Area_CalculateMovement(eArea,eArea->local.pos1,eArea->local.speed,Area_ButtonDone);

	if(eArea->local.cSoundReturn)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundReturn,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_ButtonWait(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	UseTargets(eArea, eOther);

	eArea->local.iLocalFlags = STATE_TOP;
	eArea->local.iValue = 1;

	eArea->v.think		= Area_ButtonReturn;
	eArea->v.dNextThink	= eArea->v.ltime + 4;

	if (eArea->local.sound_stop)
		Sound(eArea, CHAN_VOICE, eArea->local.sound_stop, 255, ATTN_NORM);
}

void Area_ButtonTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	if(eArea->local.iLocalFlags == STATE_UP || eArea->local.iLocalFlags == STATE_TOP)
		return;
	if((eOther->Monster.iType != MONSTER_PLAYER) && eOther->v.iHealth <= 0)
		return;

	eArea->local.iLocalFlags = STATE_UP;

	Area_CalculateMovement(eArea,eArea->local.pos2,eArea->local.speed,Area_ButtonWait);

	if(eArea->local.cSoundStart)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundStart,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_ButtonUse(ServerEntity_t *eArea)
{
	if(eArea->local.iLocalFlags == STATE_UP || eArea->local.iLocalFlags == STATE_TOP)
		return;

	eArea->local.iLocalFlags = STATE_UP;

	Area_CalculateMovement(eArea,eArea->local.pos2,eArea->local.speed,Area_ButtonWait);

	if(eArea->local.cSoundStart)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundStart,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_ButtonBlocked(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	Entity_Damage(eOther, eArea, eArea->local.iDamage, 0);
}

void Area_ButtonSpawn(ServerEntity_t *eArea)
{
	int		i;
	float	fDist;
	MathVector3f_t vMoveDir;

	if(!eArea->v.spawnflags)
		eArea->v.spawnflags = 0;

	if(eArea->local.cSoundStart)
		Server_PrecacheSound(eArea->local.cSoundStart);
	if (eArea->local.sound_stop)
		Server_PrecacheSound(eArea->local.sound_stop);
	if(eArea->local.cSoundMoving)
		Server_PrecacheSound(eArea->local.cSoundMoving);
	if(eArea->local.cSoundReturn)
		Server_PrecacheSound(eArea->local.cSoundReturn);

	eArea->v.movetype = MOVETYPE_PUSH;

	eArea->Physics.iSolid = SOLID_BSP;

	// [18/5/2013] Changed to use ! check instead since it's safer here ~hogsy
	if(eArea->local.lip == 0.0f)
		eArea->local.lip = 4.0f;

	eArea->local.iValue = 0;
	eArea->local.iLocalFlags = STATE_BOTTOM;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
	Entity_SetSizeVector(eArea,eArea->v.mins,eArea->v.maxs);

	Math_VectorCopy(eArea->v.origin,eArea->local.pos1);

	Area_SetMoveDirection(eArea->v.angles,eArea->v.movedir);

	for(i = 0; i < 3; i++)
		vMoveDir[i] = (float)fabs(eArea->v.movedir[i]);

	fDist = vMoveDir[0]*eArea->v.size[0]+
			vMoveDir[1]*eArea->v.size[1]+
			vMoveDir[2]*eArea->v.size[2]-
			eArea->local.lip;

	Math_VectorMake(eArea->local.pos1,fDist,eArea->v.movedir,eArea->local.pos2);

	if(eArea->v.spawnflags != 32) // Toggle
		eArea->v.TouchFunction = Area_ButtonTouch;

	if (eArea->local.iDamage)
		Entity_SetBlockedFunction(eArea, Area_ButtonBlocked);

	eArea->v.use = Area_ButtonUse;
}

/*
	Platform
*/


void Area_PlatformDone(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	if(eArea->local.dWait >= 0)
		eArea->v.dNextThink = eArea->v.ltime + eArea->local.dWait;

	eArea->local.iLocalFlags = STATE_DOWN;
	eArea->local.iValue = 0;
	eArea->v.think	= NULL;

	if (eArea->local.sound_stop)
		Sound(eArea, CHAN_VOICE, eArea->local.sound_stop, 255, ATTN_NORM);
}

void Area_PlatformReturn(ServerEntity_t *eArea)
{
	Area_CalculateMovement(eArea,eArea->local.pos1,eArea->local.speed,Area_PlatformDone);

	if(eArea->local.cSoundReturn)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundReturn,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_PlatformWait(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	UseTargets(eArea, eOther);

	eArea->local.iLocalFlags = STATE_TOP;
	eArea->local.iValue = 1;

	eArea->v.think		= Area_PlatformReturn;

	if(eArea->local.dWait >= 0)
	eArea->v.dNextThink	= eArea->v.ltime + eArea->local.dWait;

	if (eArea->local.sound_stop)
		Sound(eArea, CHAN_VOICE, eArea->local.sound_stop, 255, ATTN_NORM);
}

void Area_PlatformTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	if(eArea->local.iLocalFlags == STATE_UP || eArea->local.iLocalFlags == STATE_TOP)
		return;
	if((eOther->Monster.iType != MONSTER_PLAYER) && eOther->v.iHealth <= 0)
		return;
	if(eArea->v.dNextThink > eArea->v.ltime)
		return;

	eArea->local.iLocalFlags = STATE_UP;

	Area_CalculateMovement(eArea,eArea->local.pos2,eArea->local.speed,Area_PlatformWait);

	if(eArea->local.cSoundStart)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundStart,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_PlatformUse(ServerEntity_t *eArea)
{
	if(eArea->local.iLocalFlags == STATE_UP || eArea->local.iLocalFlags == STATE_TOP)
		return;

	eArea->local.iLocalFlags = STATE_UP;

	Area_CalculateMovement(eArea,eArea->local.pos2,eArea->local.speed,Area_PlatformWait);

	if(eArea->local.cSoundStart)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundStart,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_PlatformBlocked(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	Entity_Damage(eOther, eArea, eArea->local.iDamage, 0);
}

void Area_PlatformSpawn(ServerEntity_t *eArea)
{
	float	fDist;

	if(!eArea->v.spawnflags)
		eArea->v.spawnflags = 0;

	if(eArea->local.cSoundStart)
		Server_PrecacheSound(eArea->local.cSoundStart);
	if (eArea->local.sound_stop)
		Server_PrecacheSound(eArea->local.sound_stop);
	if(eArea->local.cSoundMoving)
		Server_PrecacheSound(eArea->local.cSoundMoving);
	if(eArea->local.cSoundReturn)
		Server_PrecacheSound(eArea->local.cSoundReturn);

	eArea->v.movetype = MOVETYPE_PUSH;

	eArea->Physics.iSolid = SOLID_BSP;

	if(eArea->local.count == 0)
		eArea->local.count = 100;
	if(eArea->local.dWait == 0.0)
		eArea->local.dWait = 3.0;
	if(eArea->local.iDamage == 0.0f)
		eArea->local.iDamage = 20;

	eArea->local.iValue = 0;
	eArea->local.iLocalFlags = STATE_BOTTOM;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
	Entity_SetSizeVector(eArea,eArea->v.mins,eArea->v.maxs);

	Math_VectorCopy(eArea->v.origin,eArea->local.pos1);
	Area_SetMoveDirection(eArea->v.angles, eArea->v.movedir);

	fDist = (float)eArea->local.count;

	Math_VectorMake(eArea->local.pos1, fDist, eArea->v.movedir, eArea->local.pos2);

	if(eArea->v.spawnflags != 32) // Toggle
		eArea->v.TouchFunction = Area_PlatformTouch;

	if (eArea->local.iDamage)
		Entity_SetBlockedFunction(eArea, Area_PlatformBlocked);

	eArea->v.use = Area_PlatformUse;
}


/*
	Climb / Ladders
*/

void Area_ClimbTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	MathVector3f_t vLadVelocity, vPlayerVec, vForward, vRight, vUp;

	float fForwardSpeed;

	if (eOther->v.waterlevel > 1)
		return;
	if (eOther->v.flags & FL_WATERJUMP)
		return;

	Math_AngleVectors(eOther->v.angles, vForward, vRight, vUp);
	Math_VectorCopy(vForward, vPlayerVec);
	Math_VectorScale(vPlayerVec, 250, vPlayerVec);

	if (eOther->v.button[2])
		Math_VectorCopy(eOther->v.velocity, vPlayerVec);

	if (eOther->local.dLadderJump > Server.dTime)
		return;

	Math_AngleVectors(eOther->v.angles, vForward, vRight, vUp);

	if (Math_DotProduct(vForward, eArea->v.movedir) < -0.5) // only continue when facing the ladder ~eukara
		return;

	// ignore 8 units of the top edge
	if (eOther->v.origin[2] + eOther->v.mins[2] + 8 >= eArea->v.absmax[2]){
		if (!(eOther->v.flags & FL_ONGROUND))
			eOther->v.flags = eOther->v.flags + FL_ONGROUND;
		return;
	}

	// null out gravity in PreThink
	eOther->local.dLadderTime = Server.dTime + 0.1;
	eOther->local.dZeroGTime = Server.dTime + 0.1;
	eOther->v.velocity[2] = 0;

	if (Math_DotProduct(vRight, eOther->v.velocity) > 25) 
	{
		Math_VectorClear(eOther->v.velocity);
		eOther->v.origin[0] += vRight[0] * 0.5f;
		eOther->v.origin[1] += vRight[1] * 0.5f;
		eOther->v.origin[2] += vRight[2] * 0.5f;
	//	printf("right  ");
		return;
	}
	else if (Math_DotProduct(vRight, eOther->v.velocity) < -25) 
	{
		Math_VectorClear(eOther->v.velocity);
		eOther->v.origin[0] -= vRight[0] * 0.5f;
		eOther->v.origin[1] -= vRight[1] * 0.5f;
		eOther->v.origin[2] -= vRight[2] * 0.5f;
	//	printf("left  ");
		return;
	}

	fForwardSpeed = Math_DotProduct(vForward, eOther->v.velocity);
	Math_VectorClear(vLadVelocity);

	// up (facing up/forward)
	if ((eOther->v.v_angle[0] <= 15) && (fForwardSpeed > 0))
	{
		//eOther->v.origin[0] -= eArea->v.movedir[0] * 0.36f;
		//eOther->v.origin[1] -= eArea->v.movedir[1] * 0.36f;
		eOther->v.origin[2] -= eArea->v.movedir[2] * 0.36f;
		vLadVelocity[2] = eOther->v.v_angle[0] * 6; // go faster when facing forward
		//printf("up (facing up/forward)  ");

		if (vLadVelocity[2] < 90)
			vLadVelocity[2] = 90; // minimum speed
	}
	// up (facing down)
	else if ((eOther->v.v_angle[0] >= 15) && (fForwardSpeed < 0))
	{
		//eOther->v.origin[0] += eArea->v.movedir[0] * 0.36f;
		//eOther->v.origin[1] += eArea->v.movedir[1] * 0.36f;
		eOther->v.origin[2] += eArea->v.movedir[2] * 0.36f;
		//printf("up (facing down)  ");

		vLadVelocity[2] = eOther->v.v_angle[0] * 4;
	}
	// down (facing up/forward)
	else if ((eOther->v.v_angle[0] <= 15) && (fForwardSpeed < 0))
	{
		//eOther->v.origin[0] += eArea->v.movedir[0] * 0.36f;
		//eOther->v.origin[1] += eArea->v.movedir[1] * 0.36f;
		eOther->v.origin[2] += eArea->v.movedir[2] * 0.36f;

		vLadVelocity[2] = eOther->v.v_angle[0] * -5;// go faster when facing forward
		//printf("down (facing up/forward)  ");

		if (vLadVelocity[2] > -80)
			vLadVelocity[2] = -80;// minimum speed
	}
	// down (facing down)
	else if ((eOther->v.v_angle[0] >= 15) && (fForwardSpeed > 0))
	{
		eOther->v.origin[0] -= eArea->v.movedir[0] * 0.36f;
		eOther->v.origin[1] -= eArea->v.movedir[1] * 0.36f;
		eOther->v.origin[2] -= eArea->v.movedir[2] * 0.36f;
		//printf("down (facing down)  ");

		vLadVelocity[2] = eOther->v.v_angle[0] * -4;
	}

	//printf("angle: %i; velo: %i\n", (int)eOther->v.v_angle[0], (int)vLadVelocity[2]);

	if (vLadVelocity[2] > 100)
		vLadVelocity[2] = 100;
	else if (vLadVelocity[2] < -1 * 100)
		vLadVelocity[2] = -1 * 100;

	// do it manually! VectorCopy won't work with this
	Math_VectorCopy(vLadVelocity, eOther->v.velocity);
}

void Area_ClimbSpawn(ServerEntity_t *eArea)
{
	Area_SetMoveDirection(eArea->v.angles, eArea->v.movedir);
	eArea->v.TouchFunction = Area_ClimbTouch;

	eArea->Physics.iSolid	= SOLID_TRIGGER;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
	eArea->v.model = 0;
}

/*
	Noclip
*/

void Area_NoclipSpawn(ServerEntity_t *eArea)
{
	eArea->v.movetype	= MOVETYPE_PUSH;
	eArea->Physics.iSolid	= SOLID_NOT;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
}

/*
	Pusher
*/

#define PUSH_ONCE 32

void Area_PushTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	// [9/12/2013] TODO: Make this optional? Would be cool to throw monsters and other crap around... ~hogsy
	if(!Entity_IsPlayer(eOther))
		return;

	Math_VectorScale(eArea->v.movedir,eArea->local.speed*10,eOther->v.velocity);

	// [9/12/2013] Corrected a mistake that was made here. ~hogsy
	if(eArea->v.spawnflags & PUSH_ONCE)
		Entity_Remove(eArea);
}

void Area_PushSpawn(ServerEntity_t *eArea)
{
	if(!eArea->local.speed)
		eArea->local.speed = 500.0f;

	Area_SetMoveDirection(eArea->v.angles, eArea->v.movedir);

	eArea->v.TouchFunction	= Area_PushTouch;
	eArea->Physics.iSolid	= SOLID_TRIGGER;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
	Entity_SetSizeVector(eArea,eArea->v.mins,eArea->v.maxs);

	eArea->v.model = 0;
}

/*
	area_kill
	Kills anything that enters the area.
*/

void Area_KillTouch(ServerEntity_t *area, ServerEntity_t *other)
{
	// Damage it, until it gibs and gets removed.
	Entity_Damage(other, area, other->v.iHealth, DAMAGE_TYPE_NORMAL);
}

void Area_KillSpawn(ServerEntity_t *area)
{
	area->Physics.iSolid = SOLID_TRIGGER;

	Entity_SetModel(area, area->v.model);
	Entity_SetOrigin(area, area->v.origin);
	Entity_SetTouchFunction(area, Area_KillTouch);
}
