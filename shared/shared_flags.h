/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __SHAREDFLAGS__
#define __SHAREDFLAGS__

#ifdef _MSC_VER
#pragma warning(disable:4100)	// unreferenced formal parameter
#pragma	warning(disable:4127)	// conditional expression is constant
#endif

//#define   PARANOID				// Speed sapping error checking.
#ifdef _WIN32
#define	KATANA_AUDIO_DIRECTSOUND	// DirectSound implementation.
#endif

#define	SHARED_FORMATS_C

/*	Build needs to be updated
	each day that work is done
	on the engine.
	Release needs to be updated
	when a build is released publically.	*/
#define ENGINE_VERSION_MAJOR	0
#define ENGINE_VERSION_MINOR	3
#define ENGINE_VERSION_BUILD	898	// 24/4/2015

#define	PATH_ENGINE		"engine"			// Directory to look in for engine specific modules / assets.
#define	PATH_LOGS		PATH_ENGINE"/logs"	// Default directory for saving logs.
#define	PATH_SPRITES	"textures/sprites/"	// Directory that contains sprites, used for particles etc.

// Still currently used for some older code; switch it all to use uint8_t instead :)
#define byte uint8_t

// [27/9/2012] TODO: These were and still are temp, remove ~hogsy
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

#define CMDLINE_LENGTH	256

#define	MAX_ENT_LEAFS	16

#define	MAX_QPATH	128	// Max length of a quake game pathname
#define	MAX_OSPATH	512	// Max length of a filesystem pathname

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

enum
{
	SOLID_NOT,		// Entity isn't solid.
	SOLID_TRIGGER,	// Entity will cause a trigger function.
	SOLID_BBOX,		// Entity is solid.
	SOLID_SLIDEBOX,	// Entity is solid and moves.
	SOLID_BSP
};

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

#define	EF_BRIGHTFIELD		1
#define	EF_MUZZLEFLASH		2
#define	EF_BRIGHTLIGHT 		4
#define	EF_DIMLIGHT 		8
#define	EF_GLOW_RED			16
#define EF_LIGHT_GREEN		64
#define	EF_MOTION_ROTATE	128				// Client-side rotation
#define EF_MOTION_FLOAT		256				// Adds a nice floating motion for the entity
#define EF_GLOW_BLUE		512
#define EF_GLOW_WHITE		1024
#define	EF_PARTICLE_BLOOD	2048
#define	EF_PARTICLE_SMOKE	8192			// Smoke trail.
#define EF_FULLBRIGHT		4096
#define	EF_GRENADE			16384			// leave a trail
#define	EF_GIB				32768			// leave a trail
#define	EF_TRACER			65536			// green split trail
#define	EF_TRACER2			262144			// orange split trail + rotate
#define	EF_TRACER3			524288			// purple trail
#define	EF_INVISIBLE		1048576			// Entity is invisible
#define EF_LIGHT_BLUE		2097152			// A constant blue dynamic light
#define EF_LIGHT_RED		4194304			// A constant red dynamic light

#endif
