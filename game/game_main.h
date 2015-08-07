/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __GAMEMAIN__
#define __GAMEMAIN__

/*
	Main header for our game module.
*/

#include "platform.h"
#include "platform_math.h"

#include "SharedFlags.h"
#include "SharedModule.h"
#include "shared_menu.h"
#include "SharedFormats.h"
#include "shared_game.h"

#include "game_resources.h"

#ifdef GAME_OPENKATANA
#define	GAME_NAME	"Decay: Tempus"
#else
#define	GAME_NAME	"Katana"
#endif

#ifdef __cplusplus
extern "C" {
#endif
	extern ModuleImport_t Engine;
#ifdef __cplusplus
};
#endif

typedef struct
{
	bool bActive;	// Is the server active?

	double	dTime,					// Server time.
			dHostFrameTime,			// Host time.
			dWaypointSpawnDelay;	// Delay before spawning another waypoint.

	ServerEntity_t	*eEntity,	// Current player.
					*eWorld;	// Pointer to entity representing the current level.

	char	*cMapAuthor,	// Map author.
			*cMapTitle;		// Map title.

	int	iLastGameMode,	// The last active gamemode.
		iClients,		// Number of connected clients.
		iMonsters;		// Number of monsters within the level.

	// Gamemode
	bool	bRoundStarted,			// Has the round started yet?
			bPlayersSpawned;		// Have the players been spawned for the current mode?
} GameServer_t;

typedef struct
{
	double		time;

	ClientEntity_t *ent;
} GameClient_t;

extern GameServer_t Server;
extern GameClient_t Client;

// [29/7/2013] Moved mode types into server_mode.h ~hogsy

// [28/7/2013] Moved door states into sv_area.c ~hogsy

#define	DEAD_NO				0	// Entity isn't dead
#define DEAD_DEAD			2	// Entity is dead
#define DEAD_RESPAWNABLE	3	// Entity can respawn

/*
	Items
*/

#define	WEAPON_NONE			-1
// [5/8/2013] Made these standard defines since maps rely on these now ~hogsy
#ifdef OPENKATANA
// Episode One
#define WEAPON_LASERS		1		// NPC weapon
#define WEAPON_KATANA		2		// Simple Katana / Melee
#define	WEAPON_DAIKATANA	3		// The Daikatana / Melee+
#define	WEAPON_IONBLASTER	4		// Basic projectile-based weapon
#define	WEAPON_C4VIZATERGO	5		// Explosive projectile weapon
#define	WEAPON_SHOTCYCLER	6		// Futuristic shotgun
#define	WEAPON_SIDEWINDER	7		// Explosive projectile weapon (+)
#define	WEAPON_SHOCKWAVE	8		// Powerful Shockwave Rifle
#define	AMMO_IONBALLS		9		// Used for the Ion Blaster
#define	AMMO_C4BOMBS		10		// Used for the C4 Vizatergo
#define	AMMO_SLUGS			11		// Used for the ShotCycler
#define	AMMO_WINDROCKET		12		// Used for the Sidewinder
#define	AMMO_SHOCKWAVE		13		// Used for the Shockwave Rifle
#define	ITEM_ATTACKBOOST	14
#define	ITEM_POWERBOOST		15
#define	ITEM_VITABOOST		16
#define	ITEM_SPEEDBOOST		17
#define	ITEM_ACROBOOST		18
#define	ITEM_HEALTHKIT		19		// Provides health
#define	ITEM_PLASTEELARMOR	20		// Provides armor
#define	ITEM_ENVIROSUIT		21		// Provides protection
#define	ITEM_OXYLUNG		22		// Provides oxygen
// Episode Four
#define	WEAPON_GLOCK		50		// Standard pistol
// Universal
#define	WEAPON_IONRIFLE		70		// Talon Brave's weapon
#endif
#define ITEM_FLAG			1000	// Neutral flag for CTF
#define	ITEM_REDFLAG		1001	// Red flag for CTF
#define	ITEM_BLUEFLAG		1002	// Blue flag for CTF

extern int	iRedScore, iBlueScore;

#ifdef __cplusplus
extern "C" {
#endif

	char *va(char *format,...);

	void Flare(vec3_t org,float r,float g,float b,float a,float scale,char *texture);

	void Sound(ServerEntity_t *ent, AudioChannel_t channel, char *sound, int volume, float attenuation);

	void SetAngle(ServerEntity_t *ent, vec3_t vAngle);

	void DrawPic(char *texture,float alpha,int x,int y,int h,int w);
	void PutClientInServer(ServerEntity_t *ent);
	void WriteByte(int mode,int c);
	void ChangeYaw(ServerEntity_t *ent);

	trace_t Traceline(ServerEntity_t *ent, vec3_t vStart, vec3_t vEnd, int type);

	void UseTargets(ServerEntity_t *ent, ServerEntity_t *other);

#ifdef __cplusplus
};
#endif

#endif
