/*	Copyright (C) 2011-2015 OldTimes Software

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

#include "server_weapon.h"
#include "server_item.h"

#ifdef GAME_OPENKATANA
#	include "openkatana/openkatana.h"
#elif GAME_ADAMAS
#	include "adamas/adamas.h"
#endif

/*
	This is where most functions between the Game and Engine can be found along
	with the main list of entities that's parsed upon spawning a server.
*/

bool	
	bIsMultiplayer = false,
	bIsCooperative = false,
	bIsDeathmatch = false;

void Server_Spawn(ServerEntity_t *seEntity);

typedef struct
{
	char	*name;

	void(*Spawn)(ServerEntity_t *seEntity);
	void(*Precache)();
} SpawnList_t;

SpawnList_t SpawnList[] =
{
	{ "worldspawn", Server_Spawn },
	{ "light", Point_LightSpawn },	// TODO: This should be made obsolete!

	// Area/Group Entities
	{ "area_breakable", Area_BreakableSpawn },
	{ "area_button", Area_ButtonSpawn },
	{ "area_changelevel", Area_ChangeLevel },
	{ "area_climb", Area_ClimbSpawn },
	{ "area_detail", Area_DetailSpawn },
	{ "area_door", Area_DoorSpawn },
	{ "area_door_rotate", Area_DoorSpawn },
	{ "area_noclip", Area_NoclipSpawn },
	{ "area_push", Area_PushSpawn },
	{ "area_debris", Area_PushableSpawn },		// For compatability.
	{ "area_pushable", Area_PushableSpawn },
	{ "area_platform", Area_PlatformSpawn },
	{ "area_rotate", Area_RotateSpawn },
	{ "area_trigger", Area_TriggerSpawn },
	{ "area_wall", Area_WallSpawn },
	{ "area_kill", Area_KillSpawn },
//	{ "area_playerspawn", Area_PlayerSpawn },

	// Point Entities
	{ "point_light", Point_LightSpawn },
	{ "point_sprite", Point_SpriteSpawn },
	{ "point_ambient", Point_AmbientSpawn },
	{ "point_bot", Bot_Spawn },
	{ "point_damage", Point_DamageSpawn },
	{ "point_effect", Point_EffectSpawn },
	{ "point_flare", Point_FlareSpawn },
	{ "point_item", Item_Spawn },
	{ "point_lightstyle", Point_LightstyleSpawn },
	{ "point_logic", Point_LogicSpawn },
	{ "point_message", Point_MessageSpawn },
	{ "point_monster", Point_MonsterSpawn },
	{ "point_multitrigger", Point_MultiTriggerSpawn },
	{ "point_vehicle", Point_VehicleSpawn },
	{ "point_null", Point_NullSpawn },
	{ "point_particle", Point_ParticleSpawn },
	{ "point_prop", Point_PropSpawn },
	{ "point_sound", Point_SoundSpawn },
	{ "point_start", Point_Start },
	{ "point_shake", Point_ShakeSpawn },
	{ "point_teleport", Point_TeleportSpawn },
	{ "point_timedtrigger", Point_TimedTriggerSpawn },
	{ "point_waypoint", Point_WaypointSpawn },
	{ "point_explode", Point_ExplodeSpawn },
	{ "point_decoration", Point_DecorationSpawn },

#ifdef GAME_OPENKATANA
	{ "decoration_barrel", Barrel_Spawn },

	{ "item_health", Health_Spawn },

	{ "monster_inmater", Point_MonsterSpawn },
	{ "monster_lasergat", Point_MonsterSpawn },
	{ "monster_prisoner", Point_MonsterSpawn },
#elif GAME_ADAMAS
	{ "monster_roller", Roller_Spawn, Roller_Precache },
#endif

	{	NULL	}
};

ConsoleVariable_t cvServerPlayerModel = { "server_playermodel", MODEL_PLAYER, false, true, "Sets the main server-side player model." };
ConsoleVariable_t cvServerRespawnDelay = { "server_respawndelay", "40", false, true, "Sets the amount of time until a player respawns." };
ConsoleVariable_t cvServerSkill = { "server_skill", "1", false, true, "The level of difficulty." };
ConsoleVariable_t cvServerSelfDamage = { "server_selfdamage", "0", false, true, "If enabled, your weapons can damage you." };
ConsoleVariable_t cvServerMaxHealth = { "server_maxhealth", "200", false, true, "Sets the max amount of health." };
ConsoleVariable_t cvServerDefaultHealth = { "server_defaulthealth", "100", false, true, "Changes the default amount of health." };
ConsoleVariable_t cvServerMonsters = { "server_monsters", "1", false, true, "If enabled, monsters can spawn." };
ConsoleVariable_t cvServerMaxScore = { "server_maxscore", "20", false, true, "Max score before the round ends." };
ConsoleVariable_t cvServerGameMode = { "server_gamemode", "0", false, true, "Sets the active mode of play." };
ConsoleVariable_t cvServerGameTime = { "server_gametime", "120", false, true, "Time before the round ends." };
ConsoleVariable_t cvServerGameClients = { "server_gameclients", "2", false, true, "Number of clients before round starts." };
ConsoleVariable_t cvServerWaypointDelay = { "server_waypointdelay", "5.0", false, true, "Delay before attempting to spawn another waypoint." };
ConsoleVariable_t cvServerWaypointSpawn = { "server_waypointspawn", "0", false, true, "if enabled, waypoints autospawn." };
ConsoleVariable_t cvServerWaypointParse = { "server_waypointparse", "", false, true, "Overrides the default path to load waypoints from." };
ConsoleVariable_t cvServerAim = { "server_aim", "1.0", false, true, "Enables auto-aim." };
ConsoleVariable_t cvServerGravityTweak = { "server_gravityamount", "1.0", false, true, "Gravity modifier." };
ConsoleVariable_t cvServerGravity = { "server_gravity", "600.0", false, true, "Overall gravity." };
#ifdef GAME_OPENKATANA
// By default bots spawn in OpenKatana for both SP and MP.
ConsoleVariable_t cvServerBots = { "server_bots", "1", false, true, "Can enable and disable bots." };
#else
ConsoleVariable_t cvServerBots = { "server_bots", "0", false, true, "Can enable and disable bots." };
#endif

void Server_SetGameMode(void)
{
	// Don't continue if we're already using this mode.
	if(Server.iLastGameMode == cvServerGameMode.iValue)
		return;

	if(cvServerGameMode.value >= MODE_NONE || cvServerGameMode.value < MODE_SINGLEPLAYER)
	{
		Engine.Con_Warning("Attempted to set unknown game mode! Reverting to singleplayer.\n");
		Engine.Cvar_SetValue("server_gamemode", MODE_SINGLEPLAYER);
	}

	if(Server.iLastGameMode != cvServerGameMode.value && Server.bActive)
		Engine.Con_Printf("Gamemode will be changed on next map.\n");

	// Keep OldMode up to date!
	Server.iLastGameMode = cvServerGameMode.iValue;
}

/*	Called by the engine.
*/
void Server_Initialize(void)
{
	Engine.Cvar_RegisterVariable(&cvServerSkill, NULL);
	Engine.Cvar_RegisterVariable(&cvServerSelfDamage, NULL);
	Engine.Cvar_RegisterVariable(&cvServerMaxHealth, NULL);
	Engine.Cvar_RegisterVariable(&cvServerMonsters, NULL);
	Engine.Cvar_RegisterVariable(&cvServerMaxScore, NULL);
	Engine.Cvar_RegisterVariable(&cvServerGameMode, Server_SetGameMode);
	Engine.Cvar_RegisterVariable(&cvServerPlayerModel, NULL);
	Engine.Cvar_RegisterVariable(&cvServerWaypointDelay, NULL);
	Engine.Cvar_RegisterVariable(&cvServerWaypointSpawn, NULL);
	Engine.Cvar_RegisterVariable(&cvServerBots, NULL);
	Engine.Cvar_RegisterVariable(&cvServerDefaultHealth, NULL);
	Engine.Cvar_RegisterVariable(&cvServerGameTime, NULL);
	Engine.Cvar_RegisterVariable(&cvServerGameClients, NULL);
	Engine.Cvar_RegisterVariable(&cvServerGravityTweak, NULL);
	Engine.Cvar_RegisterVariable(&cvServerGravity, NULL);
	Engine.Cvar_RegisterVariable(&cvServerRespawnDelay, NULL);
	Engine.Cvar_RegisterVariable(&cvServerAim, NULL);

	Server.bActive = false;							// We're not active when we've only just initialized.
	Server.iLastGameMode = cvServerGameMode.iValue;	// Last mode is equal to the current mode initially.
}

void Server_Spawn(ServerEntity_t *seEntity)
{
	Server.eWorld = seEntity;

	// Set defaults.
	Server.dWaypointSpawnDelay = ((double)cvServerWaypointDelay.value);
	Server.bRoundStarted =
	Server.bPlayersSpawned = false;
	Server.iMonsters = 0;

	// Set these to their defaults.
	bIsDeathmatch	= false;
	bIsCooperative	= false;
	bIsMultiplayer	= false;

	if (cvServerGameMode.iValue != MODE_SINGLEPLAYER)
	{
		bIsMultiplayer = true;

		if (cvServerGameMode.iValue == MODE_DEATHMATCH)
			bIsDeathmatch = true;
		else if (cvServerGameMode.iValue == MODE_COOPERATIVE)
			bIsCooperative = true;
	}
	else
		// Round has always immediately started in single player.
		Server.bRoundStarted = true;

	// Initialize waypoints.
	Waypoint_Initialize();

	Item_Precache();
	Weapon_Precache();

	Server_PrecacheSound("misc/deny.wav");
	Server_PrecacheSound(BASE_SOUND_TALK0);
	Server_PrecacheSound(BASE_SOUND_TALK1);
	Server_PrecacheSound(BASE_SOUND_TALK2);
	Server_PrecacheSound("misc/gib1.wav");

#ifndef GAME_ADAMAS
	// Physics
	Server_PrecacheSound(PHYSICS_SOUND_SPLASH);
	Server_PrecacheSound(PHYSICS_SOUND_BODY);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET0);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET1);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET2);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET3);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET4);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET5);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET6);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET7);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET8);
	Server_PrecacheSound(PHYSICS_SOUND_RICOCHET9);
	Server_PrecacheSound("fx/explosion1.wav");
	Server_PrecacheSound("fx/explosion2.wav");
	Server_PrecacheSound("fx/explosion3.wav");
	Server_PrecacheSound("fx/explosion4.wav");
	Server_PrecacheSound("fx/explosion5.wav");
	Server_PrecacheSound("fx/explosion6.wav");
	Server_PrecacheSound(PHYSICS_SOUND_CONCRETESTEP0);
	Server_PrecacheSound(PHYSICS_SOUND_CONCRETESTEP1);
	Server_PrecacheSound(PHYSICS_SOUND_CONCRETESTEP2);
	Server_PrecacheSound(PHYSICS_SOUND_CONCRETESTEP3);
	Server_PrecacheModel(PHYSICS_MODEL_GIB0);
	Server_PrecacheModel(PHYSICS_MODEL_GIB1);
	Server_PrecacheModel(PHYSICS_MODEL_GIB2);
	Server_PrecacheModel(PHYSICS_MODEL_GIB3);

	// Effects
	Server_PrecacheSound(SOUND_EXPLODE_UNDERWATER0);
	Server_PrecacheSound(SOUND_EXPLODE0);
	Server_PrecacheSound(SOUND_EXPLODE1);
	Server_PrecacheSound(SOUND_EXPLODE2);
#endif
	
	// Player
	Server_PrecacheModel(cvServerPlayerModel.string);

#ifdef GAME_OPENKATANA
	// Particles
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD0);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD1);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD2);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD3);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE0);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE1);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE2);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE3);

	// Player
	Server_PrecacheSound(PLAYER_SOUND_JUMP0);
	Server_PrecacheSound(PLAYER_SOUND_JUMP1);
	Server_PrecacheSound(PLAYER_SOUND_JUMP2);
	Server_PrecacheSound(PLAYER_SOUND_JUMP3);
	Server_PrecacheSound(PLAYER_SOUND_PAIN0);
	Server_PrecacheSound(PLAYER_SOUND_PAIN1);
	Server_PrecacheSound(PLAYER_SOUND_PAIN2);
	Server_PrecacheSound(PLAYER_SOUND_PAIN3);
	Server_PrecacheSound(PLAYER_SOUND_PAIN4);
	Server_PrecacheSound(PLAYER_SOUND_PAIN5);
	Server_PrecacheSound(PLAYER_SOUND_PAIN6);
	Server_PrecacheSound(PLAYER_SOUND_PAIN7);
	Server_PrecacheSound(PLAYER_SOUND_PAIN8);
	Server_PrecacheSound(PLAYER_SOUND_PAIN9);
	Server_PrecacheSound(PLAYER_SOUND_PAIN10);
	Server_PrecacheSound(PLAYER_SOUND_PAIN11);
	Server_PrecacheSound(PLAYER_SOUND_DEATH0);
	Server_PrecacheSound(PLAYER_SOUND_DEATH1);
	Server_PrecacheSound(PLAYER_SOUND_DEATH2);

	Server_PrecacheModel("models/blip.md2");

	Engine.Server_PrecacheResource(RESOURCE_SPRITE, "poison");
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, "spark");
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, "spark2");
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, "ice");
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, "zspark");
#endif

#ifdef DEBUG_WAYPOINT
	Server_PrecacheModel(WAYPOINT_MODEL_BASE);
	Server_PrecacheModel(WAYPOINT_MODEL_CLIMB);
	Server_PrecacheModel(WAYPOINT_MODEL_ITEM);
	Server_PrecacheModel(WAYPOINT_MODEL_JUMP);
	Server_PrecacheModel(WAYPOINT_MODEL_SPAWN);
	Server_PrecacheModel(WAYPOINT_MODEL_SWIM);
	Server_PrecacheModel(WAYPOINT_MODEL_WEAPON);
#endif

	// Precache any multiplayer content.
	if(bIsMultiplayer)
	{
#ifdef GAME_OPENKATANA
		Server_PrecacheSound("items/respawn.wav");

		// If we're in Vektar mode, then spawn the Vektar!
		if(cvServerGameMode.iValue == MODE_VEKTAR)
			Vektar_Spawn();
#endif
	}

	Server_WorldLightStyle(0, "m");
	Server_WorldLightStyle(1, "mmnmmommommnonmmonqnmmo");
	Server_WorldLightStyle(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");
	Server_WorldLightStyle(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");
	Server_WorldLightStyle(4, "mamamamamama");
	Server_WorldLightStyle(5, "jklmnopqrstuvwxyzyxwvutsrqponmlkj");
	Server_WorldLightStyle(6, "nmonqnmomnmomomno");
	Server_WorldLightStyle(7, "mmmaaaabcdefgmmmmaaaammmaamm");
	Server_WorldLightStyle(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");
	Server_WorldLightStyle(9, "aaaaaaaazzzzzzzz");
	Server_WorldLightStyle(10, "mmamammmmammamamaaamammma");
	Server_WorldLightStyle(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");
	Server_WorldLightStyle(32, "a");
}

/*	Called by the engine.
*/
bool Server_SpawnEntity(ServerEntity_t *entity)
{
	SpawnList_t *spawn;

	if (!entity->v.cClassname)
	{
		Engine.Con_Warning("Failed to get classname!\n");
		return false;
	}

	for (spawn = SpawnList; spawn->name; spawn++)
		if (!strcmp(spawn->name, entity->v.cClassname))
		{
			if (spawn->Precache)
				spawn->Precache();

			spawn->Spawn(entity);
			return true;
		}

	Engine.Con_Warning("Entity doesn't have a spawn function! (%s)\n", entity->v.cClassname);
	return false;
}

/*	Called by the engine.
*/
void Server_PreFrame(void)
{
	Waypoint_Frame();

#ifdef GAME_OPENKATANA
	Deathmatch_Frame();
#elif GAME_ADAMAS
#endif
}

/*	Called by the engine.
	Called per-frame for each entity just before physics.
*/
void Server_EntityFrame(ServerEntity_t *entity)
{
	Monster_Frame(entity);
}

/*	Called by the engine.
	Called for the "kill" command.
*/
void Server_KillClient(ServerEntity_t *eClient)
{
	if (eClient->Monster.state != MONSTER_STATE_DEAD)
		Entity_Damage(eClient, eClient, eClient->v.iHealth, DAMAGE_TYPE_NORMAL);
}
