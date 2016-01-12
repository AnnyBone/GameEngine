/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
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

#ifndef SHARED_FLAGS_H
#define SHARED_FLAGS_H

#ifdef _MSC_VER
#pragma warning(disable:4100)	// unreferenced formal parameter
#pragma	warning(disable:4127)	// conditional expression is constant
#endif

//#define   PARANOID				// Speed sapping error checking.
#ifdef _WIN32
#define	KATANA_AUDIO_DIRECTSOUND	// DirectSound implementation.
#endif

#define	SHARED_FORMATS_C

#define	PATH_ENGINE		"engine"			// Directory to look in for engine specific modules / assets.
#define	PATH_SPRITES	"textures/sprites/"	// Directory that contains sprites, used for particles etc.
#define	PATH_RESOURCES	"resource/"

// Still currently used for some older code; switch it all to use uint8_t instead :)
#define byte uint8_t

enum
{
	SERVER_CLIENTPOSTTHINK,
	SERVER_PLAYERPRETHINK,
	SERVER_CLIENTCONNECT,
	SERVER_CLIENTDISCONNECT,
	SERVER_CLIENTKILL,
	SERVER_SETCHANGEPARMS,
	SERVER_SETNEWPARMS
};

enum
{
	MSG_BROADCAST,	// Unreliable to all
	MSG_ONE,		// Reliable to one (msg_entity)
	MSG_ALL,		// Reliable to all
	MSG_INIT		// Write to the init string
};

typedef enum
{
	EV_STRING,
	EV_FLOAT,
	EV_DOUBLE,
	EV_VECTOR,
	EV_VECTOR4,
	ev_entity,
	EV_INTEGER,
	EV_BOOLEAN,

	EV_NONE
} DataType_t;

#define CMDLINE_LENGTH	256

#define	MAX_ENT_LEAFS	16

#define	MAX_QPATH	128	// Max length of a quake game pathname
#define	MAX_OSPATH	512	// Max length of a filesystem pathname TODO: Replace with platform library equivalent!

#define	MAX_MSGLEN		32000	// max length of a reliable message
#define	MAX_DATAGRAM	32000	// max length of unreliable message

/*	These have to be the same
	as they are engine-side so
	do not touch!
*/
enum
{
	RESOURCE_MODEL,		// States that the precache should be used for a model.
	RESOURCE_SPRITE,	// States that the precache should be used for a sprite.
	RESOURCE_SOUND,		// States that the precache should be used for a sound.
	RESOURCE_FONT,		// States that the precache should be used for a font.
	RESOURCE_MATERIAL,	// States that the precache should be used for a material.
	RESOURCE_TEXTURE	// States that the precache should be used for a texture.
};

/*	Attenuation is used for the
	Sound function and that
	alone.
*/
#define ATTN_NONE	0	// Played everywhere.
#define ATTN_NORM	1	// Large radius.
#define ATTN_IDLE	2	// Small radius.
#define ATTN_STATIC	3	// Medium radius.

/*	Different sound channels.
*/
typedef enum
{
	CHAN_AUTO,		// Sets the sound to a channel automatically (not recommended).
	CHAN_WEAPON,	// Clearly used for weapons.
	CHAN_VOICE,		// Usually used for the players grunts and groans.
	CHAN_ITEM,		// Clearly used for items in the world.
	CHAN_BODY		// Usually used for footsteps and such.
} AudioChannel_t;

// TODO: Move into shared_game!
enum
{
	MOVETYPE_NONE,			// Never moves
	MOVETYPE_WALK,			// gravity
	MOVETYPE_STEP,			// gravity, special edge handling
	MOVETYPE_FLY,			// Fly
	MOVETYPE_TOSS,			// gravity
	MOVETYPE_PUSH,			// no clip to world, push and crush
	MOVETYPE_NOCLIP,		// No colliding with world
	MOVETYPE_FLYMISSILE,	// extra size to monsters
	MOVETYPE_BOUNCE,		// Bounces upon hitting the ground
	MOVETYPE_FLYBOUNCE,		// Fly and bounce
	MOVETYPE_PHYSICS		// Sets the entity to use realistic physics
};

#define	MOVE_NORMAL		0
#define	MOVE_NOMONSTERS	1
#define	MOVE_MISSILE	2

#define	FL_FLY				1		// Entity can fly.
#define	FL_SWIM				2		// Entity can swim.
#define FL_ANGLEHACK		4		// Used for setting up dumb angles from editor.
#define	FL_INWATER			16		// Entity is in the water.
#define	FL_GODMODE			64
#define	FL_NOTARGET			128
#define	FL_ITEM				256		// Entity is an item.
#define	FL_ONGROUND			512		// Entity is on the ground.
#define	FL_PARTIALGROUND	1024	// Entity is partially on the ground.
#define	FL_WATERJUMP		2048
#define	FL_JUMPRELEASED		4096	// Entity has released jump.
#define	FL_CROUCHING		8192	// Entity is crouching.

#define	EF_BRIGHTFIELD		(1 << 0)
#define	EF_MUZZLEFLASH		(1 << 1)
#define	EF_BRIGHTLIGHT 		(1 << 2)
#define	EF_DIMLIGHT 		(1 << 3)
#define	EF_GLOW_RED			(1 << 4)
#define EF_LIGHT_GREEN		(1 << 5)
#define	EF_MOTION_ROTATE	(1 << 6)	// Client-side rotation
#define EF_MOTION_FLOAT		(1 << 7)	// Adds a nice floating motion for the entity
#define EF_GLOW_BLUE		(1 << 8)	// Simple blue dlight glow.
#define EF_GLOW_WHITE		(1 << 9)	// Simple white dlight glow.
#define	EF_PARTICLE_BLOOD	(1 << 10)	// Blood particle trail.
#define	EF_PARTICLE_SMOKE	(1 << 11)	// Smoke particle trail.
#define EF_FULLBRIGHT		(1 << 12)
#define	EF_GRENADE			(1 << 13)	// leave a trail.
#define	EF_GIB				(1 << 14)	// leave a trail.
#define	EF_TRACER			(1 << 15)	// green split trail.
#define	EF_TRACER2			(1 << 16)	// orange split trail + rotate.
#define	EF_TRACER3			(1 << 17)	// purple trail.
#define	EF_INVISIBLE		(1 << 18)	// Entity is invisible.
#define EF_LIGHT_BLUE		(1 << 19)	// A constant blue dynamic light.
#define EF_LIGHT_RED		(1 << 20)	// A constant red dynamic light.

typedef enum
{
	STOC_SPAWNSPRITE
} STOCMessage_t;

#endif // !SHARED_FLAGS_H
