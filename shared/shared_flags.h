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

#pragma once

#ifdef _MSC_VER
#	pragma warning(disable:4100)	// unreferenced formal parameter
#	pragma warning(disable:4127)	// conditional expression is constant
#endif

//#define   PARANOID				// Speed sapping error checking.

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
	MSG_INIT,		// Write to the init string

	MSG_END
};

typedef enum
{
	DATA_STRING,
	DATA_FLOAT,
	DATA_DOUBLE,
	DATA_VECTOR3,
	DATA_VECTOR4,
	DATA_INTEGER,
	DATA_BOOLEAN,
	DATA_FUNCTION,

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
	MOVETYPE_PHYSICS,		// Sets the entity to use realistic physics

	MOVETYPE_END
};

#define	MOVE_NORMAL		0
#define	MOVE_NOMONSTERS	1
#define	MOVE_MISSILE	2

enum
{
	FL_FLY				= (1 << 0),		// Entity can fly.
	FL_SWIM				= (1 << 1),		// Entity can swim.
	FL_ANGLEHACK		= (1 << 2),		// Used for setting up dumb angles from editor.
	FL_INWATER			= (1 << 3),		// Entity is in the water.
	FL_GODMODE			= (1 << 4),
	FL_NOTARGET			= (1 << 5),
	FL_ITEM				= (1 << 6),		// Entity is an item.
	FL_ONGROUND			= (1 << 7),		// Entity is on the ground.
	FL_PARTIALGROUND	= (1 << 8),		// Entity is partially on the ground.
	FL_WATERJUMP		= (1 << 9),	
	FL_JUMPRELEASED		= (1 << 10),
	FL_CROUCHING		= (1 << 11),

	FL_END
};

enum
{
	EF_BRIGHTLIGHT	= (1 << 0),
	EF_DIMLIGHT		= (1 << 1),
	EF_FULLBRIGHT	= (1 << 2),

	EF_MOTION_ROTATE	= (1 << 4),		// Client-side rotation
	EF_MOTION_FLOAT		= (1 << 5),		// Adds a nice floating motion for the entity

	EF_GLOW_BLUE	= (1 << 6),			// Simple blue dlight glow.
	EF_GLOW_WHITE	= (1 << 7),
	EF_GLOW_RED		= (1 << 8),

	EF_PARTICLE_BLOOD	= (1 << 15),	// Blood particle trail.
	EF_PARTICLE_SMOKE	= (1 << 16),	// Smoke particle trail.

	EF_LIGHT_GREEN	= (1 << 20),		// A constant green dynamic light.
	EF_LIGHT_BLUE	= (1 << 21),		// A constant blue dynamic light.
	EF_LIGHT_RED	= (1 << 22),		// A constant red dynamic light.

	EF_END
};

typedef enum
{
	STOC_SPAWNSPRITE
} STOCMessage_t;

#define	SHARED_FLAGS_VERSION	(FL_END + EF_END + MOVETYPE_END + MSG_END)
