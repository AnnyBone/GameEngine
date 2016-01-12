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

#ifndef GAME_MAIN_H
#define GAME_MAIN_H

/*
	Main header for the game module.
*/

#include "platform.h"
#include "platform_math.h"
#include "platform_filesystem.h"

#include "shared_flags.h"
#include "SharedModule.h"
#include "shared_menu.h"
#include "shared_formats.h"
#include "shared_game.h"
#include "shared_engine.h"

#include "game_resources.h"

#ifdef GAME_OPENKATANA
#	define GAME_NAME	"Decay: Tempus"
#elif GAME_ADAMAS
#	define GAME_NAME	"Adamas"
#else
#	define GAME_NAME	"Katana"
#endif

#ifdef __cplusplus
extern "C" {
#endif
	extern ModuleImport_t Engine;
#ifdef __cplusplus
};
#endif

#define	Console_Warning(a) Engine.Con_Warning(a);

typedef struct
{
	bool bActive;	// Is the server active?

	double	
		dTime,					// Server time.
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

	bool			skycam;				// Is the map using a skycam?
	MathVector3f_t	skycam_position;	// What's its position?

	// Gamemode
	bool	round_started,			// Has the round started yet?
			players_spawned;		// Have the players been spawned for the current mode?
} GameServer_t;

typedef struct
{
	double		time;

	ClientEntity_t *ent;
} GameClient_t;

extern GameServer_t Server;
extern GameClient_t Client;

#define	DEAD_NO				0	// Entity isn't dead
#define DEAD_DEAD			2	// Entity is dead
#define DEAD_RESPAWNABLE	3	// Entity can respawn

/*
	Items
*/

#define	WEAPON_NONE			-1
#ifdef GAME_OPENKATANA
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
#elif GAME_ADAMAS
#	define ITEM_LIFE	50
#endif
#define ITEM_FLAG			1000	// Neutral flag for CTF
#define	ITEM_REDFLAG		1001	// Red flag for CTF
#define	ITEM_BLUEFLAG		1002	// Blue flag for CTF

extern int	iRedScore, iBlueScore;	// TODO: Move these somewhere else, likely into the gamemode stuff...

#ifdef __cplusplus
extern "C" {
#endif

	char *va(char *format,...);

	void Flare(MathVector3f_t org,float r,float g,float b,float a,float scale,char *texture);

	void Sound(ServerEntity_t *ent, AudioChannel_t channel, char *sound, int volume, float attenuation);

	void SetAngle(ServerEntity_t *ent, MathVector3f_t vAngle);
	void PutClientInServer(ServerEntity_t *ent);
	void WriteByte(int mode,int c);
	void ChangeYaw(ServerEntity_t *ent);

	trace_t Traceline(ServerEntity_t *ent, MathVector3f_t vStart, MathVector3f_t vEnd, int type);

	void UseTargets(ServerEntity_t *ent, ServerEntity_t *other);

#ifdef __cplusplus
};
#endif

#endif	// !GAME_MAIN_H
