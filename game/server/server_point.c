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

#include "server_vehicle.h"
#include "server_physics.h"

/*
	Point entities.

	This file contains point-based entity code,
	which can NOT be assigned to brushes. - eukos

	point_monster 		- Used for placing actors/monsters/enemies
	point_vehicle 		- Used for placing vehicles
	point_start			- Entity for player start position
	point_particle		- Spawns particles
	point_flare			- Created a flare/corona
	point_light			- Casts light onto the lightmap
	point_dynamiclight	- Entity which casts light on the nearby lightmap
	point_explode		- Creates a target explosion
	point_ambient		- Plays ambient (looping) sounds
	point_camera		- Switches the users view to its own
	point_waypoint		- Waypoint for the AI
	point_sound			- Plays a sound, with different attentuations
	point_decoration	- Floating model entity with no collision
	point_message		- Displays various types of messages (local, global) on the screen
	point_teleport		- Teleports the user to a target
	point_prop			- Decoration that can break and has physics
	point_shake			- Substitute for quakes
	point_effect		- Spawns one of the many stock effects
	point_damage		- Hurts the user
	point_lightstyle	- Manipulates lightstyles
	point_multitrigger	- Trigger multiple entities by daisy-chaining them
	point_timedtrigger	- Triggers an entity after a specified delay
	point_logic			- Compilation of basic logic-gates
*/

enum
{
	PARTICLE_DEFAULT,
	PARTICLE_STACK
};

void Point_NullSpawn(ServerEntity_t *eEntity)
{
	// Remove if there is no name given!
	if (!eEntity->v.cName || (eEntity->v.cName[0] == ' '))
	{
		Entity_Remove(eEntity);
		return;
	}

	Entity_SetOrigin(eEntity,eEntity->v.origin);
}

/*
	Sky Camera
*/

void Point_SkyCameraSpawn(ServerEntity_t *entity)
{
	if (Server.skycam)
	{
		Engine.Con_Warning("Multiple sky cameras on level! (%i %i %i)\n", (int)entity->v.origin[0], (int)entity->v.origin[1], (int)entity->v.origin[2]);
		return;
	}

	Math_VectorCopy(entity->v.origin, Server.skycam_position);

	// Enable skycam, which in turn will let us know to inform the client about it.
	Server.skycam = true;

	Entity_Remove(entity);
}

/**/

#ifdef GAME_OPENKATANA
void Prisoner_Spawn(ServerEntity_t *ePrisoner);	// [2/10/2012] See monster_prisoner.c ~hogsy
void LaserGat_Spawn(ServerEntity_t *eLaserGat);	// [14/2/2013] See monster_lasergat.c ~hogsy
void Inmater_Spawn(ServerEntity_t *eInmater);		// [3/3/2013] See monster_inmater.c ~hogsy
void ScanBot_Spawn(ServerEntity_t *entity);
#elif GAME_ADAMAS
void Hurler_Spawn(ServerEntity_t *eHurler);
#endif

void Point_MonsterSpawn(ServerEntity_t *eMonster)
{
	if (cvServerMonsters.value <= 0)
	{
		Entity_Remove(eMonster);
		return;
	}

	// Set its origin and angles...
	Entity_SetOrigin(eMonster, eMonster->v.origin);
	Entity_SetAngles(eMonster, eMonster->v.angles);

	Server.iMonsters++;

	switch(eMonster->local.style)
	{
#ifdef GAME_OPENKATANA
	case MONSTER_PRISONER:
		eMonster->v.cClassname = "monster_prisoner";
		Prisoner_Spawn(eMonster);
		break;
	case MONSTER_LASERGAT:
		eMonster->v.cClassname = "monster_lasergat";
		LaserGat_Spawn(eMonster);
		break;
	case MONSTER_INMATER:
		eMonster->v.cClassname = "monster_inmater";
		Inmater_Spawn(eMonster);
		break;
	case MONSTER_SCANBOT:
		eMonster->v.cClassname = "monster_scanbot";
		ScanBot_Spawn(eMonster);
		break;
#elif GAME_ADAMAS
	case MONSTER_HURLER:
		eMonster->v.cClassname = "monster_hurler";
		Hurler_Spawn(eMonster);
		break;
#endif
	default:
		Engine.Con_Warning("Invalid monster type (%i)!\n",eMonster->local.style);

		// Reduce the monster count. ~hogsy
		Server.iMonsters--;

		Entity_Remove(eMonster);
	}
}

#ifdef GAME_ICTUS
void Rover_Spawn(ServerEntity_t *eRover);
#endif

void Point_VehicleSpawn(ServerEntity_t *eVehicle)
{
	switch(eVehicle->local.style)
	{
#ifdef GAME_ICTUS
	case VEHICLE_TYPE_ROVER:
		Rover_Spawn(eVehicle);
		break;
#endif
	default:
		Engine.Con_Warning("Invalid vehicle type! (%)\n",eVehicle->local.style);
		Entity_Remove(eVehicle);
	}
}

/*
	Player Start
*/

void Point_Start(ServerEntity_t *ent)
{
	switch((int)cvServerGameMode.value)
	{
	case MODE_SINGLEPLAYER:
		if(	(ent->local.style != INFO_PLAYER_START)		&&
			(ent->local.style != INFO_PLAYER_MIKIKO)	&&
			(ent->local.style != INFO_PLAYER_SUPERFLY))
		{
			Engine.Con_Warning("Invalid start style! (%i)\n",ent->local.style);

			Entity_Remove(ent);
			return;
		}

#ifdef GAME_OPENKATANA
		if(ent->local.style == INFO_PLAYER_SUPERFLY)
		{
			ent->local.style = BOT_COMPANION;
			Bot_Spawn(ent);
		}
		else if(ent->local.style == INFO_PLAYER_MIKIKO)
		{
			ent->local.style = BOT_COMPANION;
			Bot_Spawn(ent);
		}
		break;
	case MODE_CAPTURETHEFLAG:
		if((ent->local.style != INFO_PLAYER_CTF) || !ent->local.pTeam)
		{
			Engine.Con_Warning("Invalid start style! (%i)\n",ent->local.style);

			Entity_Remove(ent);
			return;
		}

		if(ent->local.pTeam == TEAM_RED)
			ent->v.cClassname = "point_start_red";
		else
			ent->v.cClassname = "point_start_blue";
		break;
	case MODE_COOPERATIVE:
		if(	(ent->local.style != INFO_PLAYER_START)		&&
			(ent->local.style != INFO_PLAYER_MIKIKO)	&&
			(ent->local.style != INFO_PLAYER_SUPERFLY)	&&
			(ent->local.style != INFO_PLAYER_COOP))
		{
			Engine.Con_Warning("Invalid start style! (%i)\n",ent->local.style);

			Entity_Remove(ent);
			return;
		}
		break;
	case MODE_DEATHMATCH:
	case MODE_VEKTAR:
		if(ent->local.style != INFO_PLAYER_DEATHMATCH)
		{
			Engine.Con_Warning("Invalid start style! (%i)\n",ent->local.style);

			Entity_Remove(ent);
			return;
		}

		ent->v.cClassname = "point_start_deathmatch";
		break;
#else
		break;
#endif
	default:
		Engine.Con_Warning("Failed to set up spawn points, unknown game type! (%i)\n",cvServerGameMode.value);
		
		Entity_Remove(ent);
		return;
	}

	// Create a waypoint here so bots can try to avoid.
	Waypoint_Spawn(ent->v.origin, WAYPOINT_SPAWN);
}

/*
	Particle Emitter
*/

void Point_ParticleEmit(ServerEntity_t *ent)
{
	Engine.Particle(ent->v.origin,ent->v.velocity,ent->Model.fScale,ent->v.model,ent->local.count);

	ent->v.think		= Point_ParticleEmit;
	ent->v.dNextThink	= Server.dTime+ent->local.dAttackFinished;
}

void Point_ParticleTrigger(ServerEntity_t *ent)
{
	Engine.Particle(ent->v.origin,ent->v.velocity,ent->Model.fScale,ent->v.model,ent->local.count);
}

void Point_ParticleSpawn(ServerEntity_t *ent)
{
	if(ent->local.count <= 0)
		ent->local.count = 1;

	if(ent->Model.fScale <= 0)
		ent->Model.fScale = 7;

	Engine.Server_PrecacheResource(RESOURCE_SPRITE,ent->v.model);

	Entity_SetOrigin(ent,ent->v.origin);

	switch(ent->local.style)
	{
	case PARTICLE_DEFAULT:
		break;
	case PARTICLE_STACK:
		ent->v.velocity[2] += 15.0f;
		break;
	default:
		Engine.Con_Warning("Unknown particle type (%i)!\n",ent->local.style);
	}

	if(ent->local.dAttackFinished > 0)
	{
		ent->v.think		= Point_ParticleEmit;
		ent->v.dNextThink	= Server.dTime+ent->local.dAttackFinished;
	}
	else
		ent->v.use = Point_ParticleTrigger;
}

/*
	Flare
*/

void Point_FlareSpawn(ServerEntity_t *eFlare)
{
	Engine.Server_PrecacheResource(RESOURCE_SPRITE,eFlare->v.model);

	Entity_SetOrigin(eFlare,eFlare->v.origin);

//	Flare(ent->v.origin,ent->local.red,ent->local.green,ent->local.blue,ent->alpha,ent->local.scale,ent->v.model);
}

/*
	Light
*/

#define LIGHT_OFF	1

void Point_LightUse(ServerEntity_t *eLight)
{
	if(eLight->v.spawnflags & LIGHT_OFF)
	{
		if(!eLight->v.message)
			Engine.LightStyle(eLight->local.style,"m");
		else
			Engine.LightStyle(eLight->local.style,eLight->v.message);

		eLight->v.spawnflags -= LIGHT_OFF;
	}
	else
	{
		Engine.LightStyle(eLight->local.style,"a");

		eLight->v.spawnflags += LIGHT_OFF;
	}

	if(eLight->v.noise)
		Sound(eLight,CHAN_VOICE,eLight->v.noise,255,ATTN_NORM);
}

void Point_LightSpawn(ServerEntity_t *eLight)
{
	if(eLight->v.noise)
		Server_PrecacheSound(eLight->v.noise);

	if(eLight->v.message)
		Engine.LightStyle(eLight->local.style,eLight->v.message);

	eLight->v.use = Point_LightUse;

	if(eLight->v.spawnflags & LIGHT_OFF)
		Engine.LightStyle(eLight->local.style,"a");
}

/*
	Sprite
*/

void Point_SpriteSpawn(ServerEntity_t *seSprite)
{
#if 0
	Engine.WriteByte(MSG_BROADCAST, SVC_SPRITE);
	Engine.WriteCoord(MSG_BROADCAST, seSprite->v.origin[0]);
	Engine.WriteCoord(MSG_BROADCAST, seSprite->v.origin[1]);
	Engine.WriteCoord(MSG_BROADCAST, seSprite->v.origin[2]);
#endif

	Entity_Remove(seSprite);
}

/*
	Dynamic Light
*/

void Point_DynamicLightThink(ServerEntity_t *ent)
{
	ent->v.origin[2] = (float)sin(Server.dTime*2.0)*10.0f;

	ent->v.dNextThink = Server.dTime+0.1;
}

void Point_DynamicLight(ServerEntity_t *ent)
{
	Entity_SetOrigin(ent,ent->v.origin);

	ent->v.effects = EF_LIGHT_GREEN;

	ent->v.think		= Point_DynamicLightThink;
	ent->v.dNextThink	= Server.dTime+0.1;
}

/*
	Explode
*/

#define	EXPLODE_FLAG_REMOVE	1	// Remove on use.

void Point_ExplodeUse(ServerEntity_t *ePoint)
{
	Entity_RadiusDamage(ePoint, MONSTER_RANGE_NEAR, ePoint->local.iDamage, DAMAGE_TYPE_EXPLODE);

	if (Engine.Server_PointContents(ePoint->v.origin) <= BSP_CONTENTS_WATER)
		Sound(ePoint, CHAN_AUTO, SOUND_EXPLODE_UNDERWATER0, 255, ATTN_NORM);
	else
		Sound(ePoint, CHAN_AUTO, SOUND_EXPLODE, 255, ATTN_NORM);

	if (ePoint->v.spawnflags & EXPLODE_FLAG_REMOVE)
		Entity_Remove(ePoint);
}

void Point_ExplodeSpawn(ServerEntity_t *ePoint)
{
	ePoint->v.use = Point_ExplodeUse;
}

/*
	Ambient
*/

enum
{
	NONE,
	AMBIENT_RADIUS_SMALL,
	AMBIENT_RADIUS_MEDIUM,
	AMBIENT_RADIUS_LARGE,
	AMBIENT_RADIUS_EVERYWHERE
};

void Point_AmbientSpawn(ServerEntity_t *eEntity)
{
	int iAttenuation;

	if(!eEntity->local.volume)
		eEntity->local.volume = 255;

	if(!eEntity->v.noise)
	{
		Engine.Con_Warning("No sound set for point_ambient! (%i %i %i)\n",
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	switch(eEntity->local.style)
	{
		case AMBIENT_RADIUS_EVERYWHERE:
			iAttenuation = ATTN_NONE;
			break;
		case AMBIENT_RADIUS_SMALL:
			iAttenuation = ATTN_IDLE;
			break;
		case AMBIENT_RADIUS_MEDIUM:
			iAttenuation = ATTN_STATIC;
			break;
		case AMBIENT_RADIUS_LARGE:
			iAttenuation = ATTN_NORM;
			break;
		default:
			iAttenuation = ATTN_NORM;
	}

	Server_PrecacheSound(eEntity->v.noise);

	g_engine->Server_AmbientSound(eEntity->v.origin,eEntity->v.noise,eEntity->local.volume,iAttenuation);
}

/*
	Camera
*/

// [28/7/2012] Spawn flags ~hogsy
#define	CAMERA_SPAWN_ACTIVE	1	// Defines if this camera is active after spawning

void Point_CameraUse(ServerEntity_t *eEntity, ServerEntity_t *eOther)
{
		Engine.SetMessageEntity(eOther);

		Engine.WriteByte(	MSG_ONE,	SVC_SETVIEW	);
		Engine.WriteEntity(	MSG_ONE,	eEntity		);

		// [28/7/2012] Set up the camera angle ~hogsy
		Engine.WriteByte(	MSG_ONE,	SVC_SETANGLE			);
		Engine.WriteAngle(	MSG_ONE,	eEntity->v.angles[0]	);
		Engine.WriteAngle(	MSG_ONE,	eEntity->v.angles[1]	);
		Engine.WriteAngle(	MSG_ONE,	eEntity->v.angles[2]	);
}

void Point_CameraSpawn(ServerEntity_t *eEntity)
{
	if(eEntity->v.spawnflags & CAMERA_SPAWN_ACTIVE)
	{
		//Engine.SetMessageEntity(sv_player);

		Engine.WriteByte(	MSG_ONE,	SVC_SETVIEW	);
		Engine.WriteEntity(	MSG_ONE,	eEntity		);

		// [28/7/2012] Set up the camera angle ~hogsy
		Engine.WriteByte(	MSG_ONE,	SVC_SETANGLE			);
		Engine.WriteAngle(	MSG_ONE,	eEntity->v.angles[0]	);
		Engine.WriteAngle(	MSG_ONE,	eEntity->v.angles[1]	);
		Engine.WriteAngle(	MSG_ONE,	eEntity->v.angles[2]	);
	}
	else if(!eEntity->local.dWait)
		eEntity->local.dWait = 3.0;

	Entity_SetOrigin(eEntity,eEntity->v.origin);

	//eEntity->v.use = Point_CameraUse;
}

/*
	Waypoint
*/

/*	Waypoints manually placed within a level.
	Entity gets removed after the waypoint is spawned.
*/
void Point_WaypointSpawn(ServerEntity_t *eEntity)
{
	Waypoint_Spawn(eEntity->v.origin,(WaypointType_t)eEntity->local.style);

	Entity_Remove(eEntity);
}

/*
	Sound
*/

// [2/2/2013] Is this needed or could it be extended in any way? ~eukos
enum
{
	POINT_SND_LOCAL,
	POINT_SND_ENTITY,
	POINT_SND_GLOBAL
};

void Point_SoundUse(ServerEntity_t *eEntity)
{
	switch(eEntity->local.style)
	{
	case POINT_SND_ENTITY:
		Sound(eEntity->local.activator,CHAN_AUTO,eEntity->v.noise,eEntity->local.volume,ATTN_NORM);
		break;
	case POINT_SND_GLOBAL:
		Sound(eEntity,CHAN_AUTO,eEntity->v.noise,eEntity->local.volume,ATTN_NONE);
		break;
	default:
		Sound(eEntity,CHAN_AUTO,eEntity->v.noise,eEntity->local.volume,ATTN_NORM);
	}

}

void Point_SoundSpawn(ServerEntity_t *eEntity)
{
	if(!eEntity->v.noise)
	{
		Engine.Con_Warning("No noise not set for %i (%i %i %i)!\n",
			eEntity->v.cClassname,
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}
	else if(!eEntity->local.volume)
		eEntity->local.volume = 255;

	Server_PrecacheSound(eEntity->v.noise);

	Entity_SetOrigin(eEntity,eEntity->v.origin);

	eEntity->v.use = Point_SoundUse;
}

/*
	Decoration
*/

#define DECORATION_DROPTOFLOOR	1

void Point_DecorationSpawn(ServerEntity_t *eDecoration)
{
	if (eDecoration->v.model[0] == ' ')
	{
		Entity_Remove(eDecoration);
		return;
	}

	Server_PrecacheModel(eDecoration->v.model);

	Entity_SetModel(eDecoration,eDecoration->v.model);

	if(eDecoration->v.spawnflags & DECORATION_DROPTOFLOOR)
		Entity_DropToFloor(eDecoration);
}

/*
	Message
*/

enum
{
	POINT_MSG_CENTER,
	POINT_MSG_LOCALMESSAGE,
	POINT_MSG_SERVERMESSAGE,
	POINT_MSG_INFOMESSAGE
};

void Point_MessageLocal(ServerEntity_t *eEntity)
{
	if(!eEntity->local.activator || (!Entity_IsPlayer(eEntity) && eEntity->local.activator->v.iHealth <= 0))
		return;

	Engine.Server_SinglePrint(eEntity->local.activator,eEntity->v.message);
}

void Point_MessageCenter(ServerEntity_t *eEntity)
{
	if(!eEntity->local.activator || (!Entity_IsPlayer(eEntity) && eEntity->local.activator->v.iHealth <= 0))
		return;

	Engine.CenterPrint(eEntity->local.activator,eEntity->v.message);
}

void Point_MessageServer(ServerEntity_t *eEntity)
{
	Engine.Server_BroadcastPrint(eEntity->v.message);
}

void Point_InfoMessage(ServerEntity_t *eEntity)
{
	if(!eEntity->local.activator)
		return;
	else if((eEntity->Monster.iType != MONSTER_PLAYER) && eEntity->local.activator->v.iHealth <= 0)
		return;

	Engine.Server_SinglePrint(eEntity->local.activator,"New info message received");
	eEntity->local.activator->local.cInfoMessage = eEntity->v.message;
}

void Point_MessageSpawn(ServerEntity_t *eEntity)
{
	if(!eEntity->v.message)
	{
		Engine.Con_Warning("Parameter 'message' not set! (%s) (%i %i %i)\n",
			eEntity->v.cClassname,
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	Entity_SetOrigin(eEntity,eEntity->v.origin);

	switch(eEntity->local.style)
	{
	case POINT_MSG_CENTER:
		eEntity->v.use = Point_MessageCenter;
		break;
	case POINT_MSG_LOCALMESSAGE:
		eEntity->v.use = Point_MessageLocal;
		break;
	case POINT_MSG_SERVERMESSAGE:
		eEntity->v.use = Point_MessageServer;
		break;
	case POINT_MSG_INFOMESSAGE:
		eEntity->v.use = Point_InfoMessage;
		break;
	default:
		Engine.Con_Warning("Parameter 'style' not set! (%s) (%i %i %i)\n",
			eEntity->v.cClassname,
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
	}
}

/*
	Teleport
*/

void Point_TeleportUse(ServerEntity_t *eEntity)
{
	Entity_SetOrigin(eEntity->local.activator,eEntity->v.origin);

	Math_VectorCopy(eEntity->v.angles,eEntity->local.activator->v.angles);
}

void Point_TeleportSpawn(ServerEntity_t *eEntity)
{
	Entity_SetOrigin(eEntity,eEntity->v.origin);

	eEntity->v.use = Point_TeleportUse;
}

/*
	Prop
*/

enum
{
	BREAKABLE_GLASS,
	BREAKABLE_WOOD,
	BREAKABLE_ROCK,
	BREAKABLE_METAL
};

void Area_BreakableDie(ServerEntity_t *eArea, ServerEntity_t *eOther, ServerDamageType_t type);

void Point_PropTouch(ServerEntity_t *eEntity, ServerEntity_t *eOther)
{
	if(!eOther->v.iHealth)
		return;

	plVectorClear(eEntity->v.velocity);
	Math_VectorScale(eOther->v.velocity,0.25f,eEntity->v.velocity);
}

void Point_PropSpawn(ServerEntity_t *eEntity)
{
	if(!eEntity->v.model)
		Entity_Remove(eEntity);

	eEntity->v.iHealth = 10;

	if(eEntity->v.iHealth)
	{
		switch(eEntity->local.style)
		{
			case BREAKABLE_GLASS:
				Server_PrecacheSound(PHYSICS_SOUND_GLASS0);
				Server_PrecacheSound(PHYSICS_SOUND_GLASS1);
				Server_PrecacheSound(PHYSICS_SOUND_GLASS2);
				Server_PrecacheModel(PHYSICS_MODEL_GLASS0);
				Server_PrecacheModel(PHYSICS_MODEL_GLASS1);
				Server_PrecacheModel(PHYSICS_MODEL_GLASS2);
				break;
			case BREAKABLE_WOOD:
				Server_PrecacheSound(PHYSICS_SOUND_WOOD0);
				Server_PrecacheSound(PHYSICS_SOUND_WOOD1);
				Server_PrecacheSound(PHYSICS_SOUND_WOOD2);
				Server_PrecacheModel(PHYSICS_MODEL_WOOD0);
				Server_PrecacheModel(PHYSICS_MODEL_WOOD1);
				Server_PrecacheModel(PHYSICS_MODEL_WOOD2);
				break;
			case BREAKABLE_ROCK:
				Server_PrecacheSound(PHYSICS_SOUND_ROCK0);
				Server_PrecacheSound(PHYSICS_SOUND_ROCK1);
				Server_PrecacheSound(PHYSICS_SOUND_ROCK2);
				Server_PrecacheModel("models/gibs/rock_gibs1.md2");
				Server_PrecacheModel("models/gibs/rock_gibs2.md2");
				Server_PrecacheModel("models/gibs/rock_gibs3.md2");
				break;
			case BREAKABLE_METAL:
				Server_PrecacheSound(PHYSICS_SOUND_METAL0);
				Server_PrecacheSound(PHYSICS_SOUND_METAL1);
				Server_PrecacheSound(PHYSICS_SOUND_METAL2);
				Server_PrecacheModel(PHYSICS_MODEL_METAL0);
				Server_PrecacheModel(PHYSICS_MODEL_METAL1);
				Server_PrecacheModel(PHYSICS_MODEL_METAL2);
				break;
			default:
				Engine.Con_Warning("Prop with unknown style! (%i)\n",eEntity->local.style);
		}

		eEntity->v.bTakeDamage = true;
		eEntity->local.bBleed = false;

		Entity_SetKilledFunction(eEntity, Area_BreakableDie);
	}

	Server_PrecacheModel(eEntity->v.model);

	eEntity->v.movetype			= MOVETYPE_BOUNCE;
	eEntity->v.TouchFunction	= Point_PropTouch;

	eEntity->Physics.iSolid		= SOLID_BBOX;
	eEntity->Physics.fGravity	= cvServerGravity.value;
	eEntity->Physics.fMass		= 0.5f;

	Entity_SetModel(eEntity,eEntity->v.model);
	Entity_SetOrigin(eEntity,eEntity->v.origin);
	Entity_SetSize(eEntity,-16.0f,-16.0f,-24.0f,16.0f,16.0f,32.0f);
}

/*
	Shake
*/

void Point_ShakeThink (ServerEntity_t *eEntity)
{
	ServerEntity_t *eEnts = Engine.Server_FindRadius(eEntity->v.origin,10000.0f);

	if(eEntity->local.dAttackFinished < Server.dTime)
		return;

	do
	{
		if(!(eEnts->v.movetype == MOVETYPE_NONE) && !(eEnts->v.movetype == MOVETYPE_FLY))
		{
			eEnts->v.velocity[0] += Math_CRandom()*250;
			eEnts->v.velocity[1] += Math_CRandom()*250;

			eEnts->v.punchangle[0] += Math_CRandom()*8;
			eEnts->v.punchangle[2] += Math_CRandom()*8;

			if(eEnts->v.flags & FL_ONGROUND)
				eEnts->v.velocity[2] = eEntity->local.speed;
		}

		eEnts = eEnts->v.chain;

	} while(eEnts);

	eEntity->v.dNextThink = Server.dTime+eEntity->local.delay;
}

void Point_ShakeUse (ServerEntity_t *eEntity)
{
	eEntity->v.dNextThink			= Server.dTime;
	eEntity->local.dAttackFinished	= Server.dTime+eEntity->local.dWait;
}

void Point_ShakeSpawn (ServerEntity_t *eEntity)
{
	if (!eEntity->local.delay)
		eEntity->local.delay = 0.2f;

	if (!eEntity->local.speed)
		eEntity->local.speed = 300;

	if (!eEntity->local.dWait)
		eEntity->local.dWait = 5.0;

	eEntity->v.use		= Point_ShakeUse;
	eEntity->v.think	= Point_ShakeThink;
}

/*
	Effect
*/

void Point_EffectUse(ServerEntity_t *eEntity)
{
	switch(eEntity->local.style)
	{
		case 1:
			Entity_RadiusDamage(eEntity,MONSTER_RANGE_MEDIUM,eEntity->local.iDamage, eEntity->local.iDamageType);
			break;
		case 2:
			Engine.WriteByte(MSG_BROADCAST,SVC_TEMPENTITY);
			Engine.WriteByte(MSG_BROADCAST,CTE_GUNSHOT);
			Engine.WriteCoord(MSG_BROADCAST,eEntity->v.origin[0]);
			Engine.WriteCoord(MSG_BROADCAST,eEntity->v.origin[1]);
			Engine.WriteCoord(MSG_BROADCAST,eEntity->v.origin[2]);
			break;
		case 3:
			Engine.WriteByte(MSG_BROADCAST,SVC_TEMPENTITY);
			Engine.WriteByte(MSG_BROADCAST,CTE_TELEPORT);
			Engine.WriteCoord(MSG_BROADCAST,eEntity->v.origin[0]);
			Engine.WriteCoord(MSG_BROADCAST,eEntity->v.origin[1]);
			Engine.WriteCoord(MSG_BROADCAST,eEntity->v.origin[2]);
			break;
		default:
			Engine.Con_Warning("Unknown effect style! (%i)\n",eEntity->local.style);
	}

	if(eEntity->v.noise)
		Sound(eEntity,CHAN_ITEM,eEntity->v.noise,255,ATTN_NORM);
}

void Point_EffectSpawn(ServerEntity_t *eEntity)
{
	if(eEntity->v.noise)
		Server_PrecacheSound(eEntity->v.noise);

	eEntity->v.use = Point_EffectUse;

	Entity_SetOrigin(eEntity,eEntity->v.origin);
}

/*
	Damage
*/

void Point_DamageUse(ServerEntity_t *eEntity)
{
	Entity_Damage(eEntity->local.activator, eEntity, eEntity->local.iDamage, eEntity->local.style);
}

void Point_DamageSpawn(ServerEntity_t *eEntity)
{
	if(!eEntity->local.iDamage)
		eEntity->local.iDamage = 10;

	if(!eEntity->local.style)
		eEntity->local.style = DAMAGE_TYPE_NORMAL;

	eEntity->v.use = Point_DamageUse;

	Entity_SetOrigin(eEntity,eEntity->v.origin);
}

/*
	Lightstyle Switcher
*/

void Point_LightstyleDie(ServerEntity_t *eEntity)
{
	Engine.LightStyle(eEntity->local.style,"a");

	if(eEntity->v.targetname)
		UseTargets(eEntity, eEntity);
}

void Point_LightstyleUse(ServerEntity_t *eEntity)
{
	if(eEntity->v.noise)
		Sound(eEntity,CHAN_ITEM,eEntity->v.noise,255,ATTN_NORM);

	Engine.LightStyle(eEntity->local.style,eEntity->v.message);

	if(eEntity->local.dWait > 0)
	{
		eEntity->v.dNextThink	= Server.dTime+eEntity->local.dWait;
		eEntity->v.think		= Point_LightstyleDie;
	}
}

void Point_LightstyleSpawn(ServerEntity_t *eEntity)
{
	if(eEntity->v.noise)
		Server_PrecacheSound(eEntity->v.noise);

	if(!eEntity->v.message)
		eEntity->v.message = "a";

	if(!eEntity->local.style)
	{
		Engine.Con_Warning("No style set for point_lightstyle! (%i %i %i)\n",
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	eEntity->v.use = Point_LightstyleUse;

	Entity_SetOrigin(eEntity,eEntity->v.origin);
}

/*
	Multi-Trigger / Random
*/

void Point_MultiTriggerUse(ServerEntity_t *eEntity)
{
	int iRand;

	if(eEntity->local.style)
	{
		iRand = rand();

		if(iRand < 0.5)
			UseTargets(eEntity, eEntity);
		else
		{
			strcpy(eEntity->v.targetname, eEntity->v.message);
			UseTargets(eEntity, eEntity);
		}
	}
	else
	{
		UseTargets(eEntity, eEntity);
		strcpy(eEntity->v.targetname, eEntity->v.message);
		UseTargets(eEntity, eEntity);
	}
}

void Point_MultiTriggerSpawn(ServerEntity_t *eEntity)
{
	if(!eEntity->v.targetname)
	{
		Engine.Con_Warning("No targetname set for point_multitrigger! (%i %i %i)\n",
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	if(!eEntity->v.message)
	{
		Engine.Con_Warning("No message set for point_multitrigger! (%i %i %i)\n",
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	if(!eEntity->local.style)
		eEntity->local.style = 0;

	eEntity->v.use = Point_MultiTriggerUse;

	Entity_SetOrigin(eEntity,eEntity->v.origin);
}

/*
	Timed Trigger
*/

void Point_TimedTriggerThink(ServerEntity_t *eEntity)
{
	UseTargets(eEntity, eEntity);
}

void Point_TimedTriggerUse(ServerEntity_t *eEntity)
{
	eEntity->v.dNextThink	= Server.dTime+eEntity->local.dWait;
	eEntity->v.think		= Point_TimedTriggerThink;
}

void Point_TimedTriggerSpawn(ServerEntity_t *eEntity)
{
	if(!eEntity->v.targetname)
	{
		Engine.Con_Warning("No targetname set for point_timedtrigger (%f %f %f)!\n",
			eEntity->v.origin[0],
			eEntity->v.origin[1],
			eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	eEntity->v.use = Point_TimedTriggerUse;

	if(eEntity->v.spawnflags & 32)
		eEntity->v.use(eEntity);

	Entity_SetOrigin(eEntity,eEntity->v.origin);
}

/*
	Logic
*/

void Point_LogicThink(ServerEntity_t *eEntity)
{
	ServerEntity_t *eEnt1, *eEnt2 = NULL;

	eEnt1 = Engine.Server_FindEntity(Server.eWorld,eEntity->local.cTarget1,false);
	if(!eEnt1)
	{
		Engine.Con_Warning("Point_Logic: Can't find Target1!\n");
		return;
	}

	if(eEntity->local.style < 3)
	{
		eEnt2 = Engine.Server_FindEntity(Server.eWorld,eEntity->local.cTarget2,false);
		if(!eEnt2)
		{
			Engine.Con_Warning("Point_Logic: Can't find Target2!\n");
			return;
		}
	}

	switch(eEntity->local.style)
	{
	case 1:
		if(eEnt1->local.iValue && eEnt2->local.iValue)
		{
			if(eEntity->v.targetname && !eEntity->local.iValue)
				UseTargets(eEntity,eEntity->local.activator);

			eEntity->local.iValue = 1;
		}
		else
			eEntity->local.iValue = 0;
		break;
	case 2:
		if(eEnt1->local.iValue || eEnt2->local.iValue)
		{
			if(eEntity->v.targetname && eEntity->local.iValue)
				UseTargets(eEntity,eEntity->local.activator);

			eEntity->local.iValue = 1;
		}
		else
			eEntity->local.iValue = 0;
		break;
	case 3:
		if(!eEnt1->local.iValue)
			eEntity->local.iValue = 1;
		else
			eEntity->local.iValue = 0;
		break;
	}

	eEntity->v.dNextThink = Server.dTime+eEntity->local.dWait;
}

void Point_LogicSpawn(ServerEntity_t *eEntity)
{
	if(!eEntity->local.cTarget1)
	{
		Engine.Con_Warning("No target1 set for point_logic! (%i %i %i)\n",
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	if(!eEntity->local.cTarget2 && eEntity->local.style != 3)
	{
		Engine.Con_Warning("No target2 set for point_logic! (%i %i %i)\n",
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);

		Entity_Remove(eEntity);
		return;
	}

	if(!eEntity->local.style)
		eEntity->local.style = 1;

	if(!eEntity->local.dWait)
		eEntity->local.dWait = 1;

	if(eEntity->local.style != 3)
		eEntity->local.iValue = 0;
	else
		eEntity->local.iValue = 1;

	Entity_SetOrigin(eEntity,eEntity->v.origin);

	eEntity->v.think		= Point_LogicThink;
	eEntity->v.dNextThink	= Server.dTime+1.0f;
}
