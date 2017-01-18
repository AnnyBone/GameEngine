/*
Copyright (C) 2011-2017 OldTimes Software

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

#pragma once

/*	Main header for the game module.	*/

#include "platform_filesystem.h"
#include "platform_math.h"

#include "shared_base.h"
#include "shared_game.h"
#include "shared_menu.h"

#include "game_resources.h"

#ifdef GAME_OPENKATANA
#	define GAME_NAME	"Decay: Tempus"
#elif GAME_ADAMAS
#	define GAME_NAME	"Adamas"
#else
#	define GAME_NAME	"Katana"
#endif

PL_EXTERN_C

PL_EXTERN ModuleImport_t Engine;		// TODO: Obsolete!
PL_EXTERN ModuleImport_t *g_engine;

PL_EXTERN_C_END

#define	Console_Warning(a) Engine.Con_Warning(a);

typedef struct
{
	bool bActive;	// Is the server active?

	double	
		time,					// Server time.
		dHostFrameTime,			// Host time.
		dWaypointSpawnDelay;	// Delay before spawning another waypoint.

	ServerEntity_t	
		*eEntity,	// Current player.
		*eWorld;	// Pointer to entity representing the current level.

	char	
		*cMapAuthor,	// Map author.
		*cMapTitle;		// Map title.

	int	iLastGameMode,	// The last active gamemode.
		iClients,		// Number of connected clients.
		iMonsters;		// Number of monsters within the level.

	bool		skycam;				// Is the map using a skycam?
	PLVector3D	skycam_position;	// What's its position?

	// Gamemode
	bool	round_started,			// Has the round started yet?
			players_spawned;		// Have the players been spawned for the current mode?
} GameServer_t;

typedef struct
{
	double		time;

	ClientEntity_t *ent;
} GameClient_t;

PL_EXTERN_C

PL_EXTERN GameServer_t Server;
PL_EXTERN GameClient_t Client;

PL_EXTERN_C_END

#define	DEAD_NO				0	// Entity isn't dead
#define DEAD_DEAD			2	// Entity is dead
#define DEAD_RESPAWNABLE	3	// Entity can respawn

typedef enum
{
	BLOOD_TYPE_RED,
	BLOOD_TYPE_GREEN
} BloodType_t;

/*
	Items
*/

#define	ITEM_WEAPON_NONE			-1
#ifdef GAME_OPENKATANA

// Episode One
#define ITEM_WEAPON_LASERS		1		// NPC weapon
#define WEAPON_KATANA		2		// Simple Katana / Melee
#define	WEAPON_DAIKATANA	3		// The Daikatana / Melee+
#define	WEAPON_IONRIFLE		4		// Basic projectile-based weapon
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
#elif GAME_ADAMAS
#	define ITEM_LIFE	50
#endif

#define ITEM_FLAG			1000	// Neutral flag for CTF
#define	ITEM_REDFLAG		1001	// Red flag for CTF
#define	ITEM_BLUEFLAG		1002	// Blue flag for CTF

extern int	iRedScore, iBlueScore;	// TODO: Move these somewhere else, likely into the gamemode stuff...

PL_EXTERN_C

char *va(char *format,...);

void Flare(PLVector3D org,float r,float g,float b,float a,float scale,char *texture);

void Sound(ServerEntity_t *ent, AudioChannel_t channel, char *sound, int volume, float attenuation);

void SetAngle(ServerEntity_t *ent, PLVector3D vAngle);
void PutClientInServer(ServerEntity_t *ent);
void WriteByte(int mode,int c);
void ChangeYaw(ServerEntity_t *ent);

trace_t Traceline(ServerEntity_t *ent, PLVector3D vStart, PLVector3D vEnd, int type);

void UseTargets(ServerEntity_t *ent, ServerEntity_t *other);

PL_EXTERN_C_END
