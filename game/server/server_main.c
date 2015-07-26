/*	Copyright (C) 2011-2013 OldTimes Software
*/
#include "server_main.h"

#include "platform_filesystem.h"

/*
	This is where most functions between the Game and Engine can be found along
	with the main list of entities that's parsed upon spawning a server.
*/

#include "server_waypoint.h"
#include "server_weapon.h"
#include "server_item.h"

#ifdef GAME_OPENKATANA
// [20/12/2012] Include the Vektar stuff for spawning ~hogsy
#include "openkatana/mode_vektar.h"

#include "openkatana/openkatana.h"
#endif

void Server_Spawn(ServerEntity_t *seEntity);

typedef struct
{
	char	*name;
	void(*spawn)(ServerEntity_t *seEntity);
} SpawnList_t;

/*	[28/11/2012]
	Removed info_player_start for good. ~hogsy
	[29/11/2012]
	Commented out point_dynamiclit. ~hogsy
*/
SpawnList_t SpawnList[] =
{
	{ "worldspawn", Server_Spawn },
	{ "light", Point_LightSpawn },	// TODO: This should be made obsolete ~hogsy

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
	{ "area_debris", Area_PushableSpawn },	// [25/9/2013] For compatability ~hogsy
	{ "area_pushable", Area_PushableSpawn },
	{ "area_platform", Area_PlatformSpawn },
	{ "area_rotate", Area_RotateSpawn },
	{ "area_trigger", Area_TriggerSpawn },
	{ "area_wall", Area_WallSpawn },
//	{ "area_playerspawn", Area_PlayerSpawn },

	// Point Entities
	{ "point_light", Point_LightSpawn },
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

#ifdef GAME_OPENKATANA
	{ "point_decoration", Point_DecorationSpawn },
	{ "decoration_barrel", Barrel_Spawn },

	{ "monster_inmater", Point_MonsterSpawn },
	{ "monster_lasergat", Point_MonsterSpawn },
	{ "monster_prisoner", Point_MonsterSpawn },
#elif GAME_ADAMAS
#endif

	{	NULL,	NULL	}
};

ConsoleVariable_t cvServerPlayerModel = { "server_playermodel", "models/player.md2", false, true, "Sets the main server-side player model." };
ConsoleVariable_t cvServerRespawnDelay = { "server_respawndelay", "40", false, true, "Sets the amount of time until a player respawns." };
ConsoleVariable_t cvServerSkill = { "server_skill", "1", false, true, "The level of difficulty." };
ConsoleVariable_t cvServerSelfDamage = { "server_selfdamage", "0", false, true, "If enabled, your weapons can damage you." };
// [7/12/2012] Changed default maxhealth to 200 ~hogsy
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
// [19/3/2013] Replacement for the engine-side variable ~hogsy
ConsoleVariable_t cvServerGravityTweak = { "server_gravityamount", "1.0", false, true, "Gravity modifier." };
ConsoleVariable_t cvServerGravity = { "server_gravity", "600.0", false, true, "Overall gravity." };
#ifdef GAME_OPENKATANA
// [20/1/2013] By default bots spawn in OpenKatana for both SP and MP ~hogsy
ConsoleVariable_t cvServerBots = { "server_bots", "1", false, true, "Can enable and disable bots." };
#else
ConsoleVariable_t cvServerBots = { "server_bots",	"0", false,	true, "Can enable and disable bots." };
#endif

void Server_SetGameMode(void)
{
	// [7/12/2012] Don't continue if we're already using this mode ~hogsy
	if(Server.iLastGameMode == cvServerGameMode.iValue)
		return;

	if(cvServerGameMode.value >= MODE_NONE || cvServerGameMode.value < MODE_SINGLEPLAYER)
	{
		Engine.Con_Warning("Attempted to set unknown game mode! Reverting to singleplayer.\n");
		Engine.Cvar_SetValue("server_gamemode",MODE_SINGLEPLAYER);
	}

	// [5/6/2012] Moved so we give this message if the above case occurs ~hogsy
	if(Server.iLastGameMode != cvServerGameMode.value && Server.bActive)
		Engine.Con_Printf("Gamemode will be changed on next map.\n");

	// [29/3/2012] Keep OldMode up to date! ~hogsy
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

	Server.bActive = false;						// [7/12/2012] We're not active when we've only just initialized ~hogsy
	Server.iLastGameMode = cvServerGameMode.iValue;
}

/*	Used just for GIPL shit... Bleh...
*/
void Server_PrecachePlayerModel(char *ccFile)
{
	Engine.Con_Printf("EX: %s\n",ccFile);

	Server_PrecacheModel(ccFile);
}

void Server_Spawn(ServerEntity_t *seEntity)
{
	Server.eWorld = seEntity;

	// Set defaults.
	Server.dWaypointSpawnDelay = ((double)cvServerWaypointDelay.value);
	Server.bRoundStarted =
	Server.bPlayersSpawned = false; // [5/9/3024] Players have no been spawned yet ~hogsy
	Server.iMonsters = 0;
#ifdef GAME_ADAMAS
	Server.iLives = 2;
#endif

	// Set these to their defaults.
	bIsDeathmatch = false;
	bIsCooperative = false;
	bIsMultiplayer = false;

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
	
	// Player
	Server_PrecacheModel(cvServerPlayerModel.string);

	// Particles
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD0);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD1);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD2);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_BLOOD3);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE0);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE1);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE2);
	Engine.Server_PrecacheResource(RESOURCE_SPRITE, PARTICLE_SMOKE3);

#ifdef GAME_OPENKATANA	// [22/4/2013] OpenKatana specific stuff is now here instead ~hogsy
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

	// [10/4/2013] Waypoint debugging models ~hogsy
#ifdef DEBUG_WAYPOINT
	Server_PrecacheModel(WAYPOINT_MODEL_BASE);
	Server_PrecacheModel(WAYPOINT_MODEL_CLIMB);
	Server_PrecacheModel(WAYPOINT_MODEL_ITEM);
	Server_PrecacheModel(WAYPOINT_MODEL_JUMP);
	Server_PrecacheModel(WAYPOINT_MODEL_SPAWN);
	Server_PrecacheModel(WAYPOINT_MODEL_SWIM);
	Server_PrecacheModel(WAYPOINT_MODEL_WEAPON);
#endif

	// [7/3/2012] Added mode specific precaches ~hogsy
	// [31/7/2012] Changed so we precache these if it's multiplayer ~hogsy
	if(bIsMultiplayer)
	{
#ifdef GAME_OPENKATANA
		// [31/7/2012] TODO: We need an md2 version of this! ~hogsy
//		Server_PrecacheModel("models/mikiko.mdl");
		Server_PrecacheSound("items/respawn.wav");

		// [20/12/2012] If we're in Vektar mode, then spawn the Vektar! ~hogsy
		if(cvServerGameMode.iValue == MODE_VEKTAR)
			Vektar_Spawn();
#endif

		// [31/7/2013] Precache custom player models ~hogsy
		// [31/7/2013] TODO: Get actual current game directory from the engine ~hogsy
		pFileSystem_ScanDirectory("data/models/player/", ".md2", Server_PrecachePlayerModel);
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
bool Server_SpawnEntity(ServerEntity_t *seEntity)
{
	SpawnList_t *slSpawn;

	if (!seEntity->v.cClassname)
	{
		Engine.Con_Warning("Failed to get classname!\n");
		return false;
	}

	for(slSpawn = SpawnList; slSpawn->name; slSpawn++)
		if (!strcmp(slSpawn->name, seEntity->v.cClassname))
		{
			slSpawn->spawn(seEntity);
			return true;
		}

	Engine.Con_Warning("Entity doesn't have a spawn function! (%s)\n", seEntity->v.cClassname);
	return false;
}

/*	Called by the engine.
*/
void Server_StartFrame(void)
{
#ifdef GAME_OPENKATANA
	Deathmatch_Frame();
#elif GAME_ADAMAS
	// This is stupid... ~hogsy
	if(!Server.iMonsters && Server.bRoundStarted)
	{
		if(strstr(Engine.Server_GetLevelName(),"0"))
			Engine.Server_ChangeLevel("room1");
		else if(strstr(Engine.Server_GetLevelName(),"1"))
			Engine.Server_ChangeLevel("room2");
		else
		{
			Engine.Server_BroadcastPrint("You Win!!\n");
			Engine.Server_ChangeLevel("room0");
		}
	}
#endif
}

/*	Called by the engine.
	Called per-frame for each entity just before physics.
*/
void Server_EntityFrame(ServerEntity_t *eEntity)
{
	Waypoint_Frame(eEntity);
	Monster_Frame(eEntity);
}

/*	Called by the engine.
	Called for the "kill" command.
*/
void Server_KillClient(ServerEntity_t *eClient)
{
	if(eClient->Monster.iState != STATE_DEAD)
		Entity_Damage(eClient, eClient, eClient->v.iHealth, 0);
}

/*	General function for globally updating the HUD for clients.
*/
void Server_UpdateClientMenu(ServerEntity_t *eClient, int iMenuState, bool bShow)
{
	// [5/8/2013] This is who we're telling to hide/show their HUD ~hogsy
	Engine.SetMessageEntity(eClient);

	Engine.WriteByte(MSG_ONE,SVC_UPDATEMENU);
	Engine.WriteByte(MSG_ONE,iMenuState);
	Engine.WriteByte(MSG_ONE,bShow);
}
