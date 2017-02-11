/*
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

#define	SERVER_GRAVITY	600.0f

enum ServerMessage
{
	/*	TODO: Reduce all this, and then shift stuff over
		into game module.
	*/
	SVC_NOP = 1,							// Used by the engine to check if a client is still connected(?)
	SVC_DISCONNECT,							// Disconnects the current server
	SVC_UPDATESTAT,							// Updates the specified client statistic
	SVC_VERSION,							// Sends the server version to the engine for checking
	SVC_SETVIEW,							// TODO: Sets the entity to view from?
	SVC_SOUND,
	SVC_TIME,								// TODO: Sets the time? Client time? What!?
	SVC_PRINT,								// Prints a message to the console
	SVC_STUFFTEXT,							// Adds text to the console buffer
	SVC_SETANGLE,							// Sets the view angle
	SVC_SERVERINFO,							// TODO: Used to print/get information about the current server?
	SVC_LIGHTSTYLE,							// Used for lightstyles
	SVC_UPDATENAME,							// TODO: Updates the clients name in the scoreboard?
	SVC_UPDATEFRAGS,						// Clearly updates the clients frag count
	SVC_CLIENTDATA,
	SVC_STOPSOUND,							// Stops the specified sound
	SVC_UPDATECOLORS,						// Updates with player colours I assume
	SVC_PARTICLE,							// Used to create particles
	SVC_DAMAGE,								// TODO: Tells the server we've been damaged?
	SVC_SPAWNSTATIC,
	SVC_SPAWNBASELINE = 22,
	SVC_TEMPENTITY,							// Spawns a temporary entity
	SVC_SETPAUSE,							// Pauses the game
	SVC_SIGNONNUM,
	SVC_CENTERPRINT,						// Allows us to send a centered message to either everyone or just a specific client
	SVC_KILLEDMONSTER,						// Updates the clients statistic count for kills
	SVC_FOUNDSECRET,						// Updates the clients statistic count for secrets
	SVC_SPAWNSTATICSOUND,
	SVC_INTERMISSION,
	SVC_FINALE,
	SVC_CDTRACK,
	SVC_CUTSCENE		= 34,				// Prepares the client for an in-game cutscene
	SVC_SKYBOX			= 37,				// Sets the skybox for the current level
	SVC_BF				= 40,				// Executes the bf command
	SVC_FOG,								// Sets properties for the fog in the current level
	SVC_SPAWNBASELINE2,
	SVC_SPAWNSTATIC2,
	SVC_SPAWNSTATICSOUND2,
	SVC_UPDATEMENU,
	//SVC_SPRITE,

/*	Anything beyond this point
	is local.					*/
	MESSAGE_SERVER_SKYCAMERA,
	MESSAGE_SERVER_DEBUG,

	SVC_NONE
};

#define	SERVER_PROTOCOL	(1+(SVC_NONE))

#define	NUM_PING_TIMES		16
#define	NUM_SPAWN_PARMS		29

#define	SERVER_MAX_TEXTURES	512	// Maximum number of textures we can send.

#include "shared_common.h"
#include "shared_client.h"

#include "shared_server_entity.h"

typedef struct ServerEntity_s ServerEntity_t;

// Monster specific variables
typedef struct
{
	int	type;

	void(*Frame)(ServerEntity_t *entity);	// Called per-frame for the specific handling of states for each monster.

	// State Functions
	void(*Idle)(ServerEntity_t *entity);
	void(*Jump)(ServerEntity_t *entity);
	void(*Land)(ServerEntity_t *entity);
	void(*Pain)(ServerEntity_t *entity, ServerEntity_t *other, EntityDamageType_t type);

	// Current thought state
	unsigned int	state;					// Current physical state.
	unsigned int	think;					// Current thought process.
	int	commands[64];			// List of sub-commands for the monster to execute.
	unsigned int emotions[16];	// Current emotional states.

	float attack_delay;		// Delay before attempting to attack again.

	// Targets
	// TODO: Crunch this down!
	ServerEntity_t *eEnemy;		// Current enemy.
	ServerEntity_t *eOldEnemy;	// Last enemy.
	ServerEntity_t *eFriend;	// Current friend.
	ServerEntity_t *eOldFriend;	// Last friend.
	ServerEntity_t *eTarget;	// Current target.
	ServerEntity_t *eOldTarget;	// Last target.
} ServerMonsterVariables_t;

/*	These are tied in with
the different flag types
so please be careful when
modifying them.
*/
typedef enum
{
	TEAM_NEUTRAL,				// Team neutral, set for deathmatch or other non-team modes
	TEAM_RED,					// For team-based gameplay
	TEAM_BLUE,					// For team-based gameplay
	TEAM_SPECTATOR				// Fags who don't want to play
} PlayerTeam_t;

// Misc local variables
typedef struct
{
	// TODO: Move all mapping parameters to its own struct.
	char		*sound;
	float		speed;

	int	damage,	// Amount of damage to inflict.
		value;		// TODO: Make this a little less... Ambiguous?

	int			volume;		// Volume level for sound.
	float		lip;
	int			distance;
	int			style;

	double wait;

	int			count;

	char
		*cTarget1,	// First target.
		*cTarget2;	// Second target.

	char
		*cSoundStart,
		*sound_stop,
		*cSoundMoving,
		*cSoundReturn;

#ifdef GAME_OPENKATANA
	// Powerups
	double		
		power_finished, power_time,		// Power Boost.
		speed_finished, speed_time,		// Speed Boost.
		acro_finished, acro_time,		// Acro Boost.
		attackb_finished, attackb_time,	// Attack Boost.
		vita_finished, vita_time;		// Vita Boost.

	// Ammo
	int			c4_ammo,
		claw_ammo;
	int			glock_ammo, glock_ammo2;
	int			ionblaster_ammo;
	int			discus_ammo, discus_ammo2;
	int			pulserifle_ammo;
	int			hermes_ammo;
	int			cordite_ammo;
	int			barrier_ammo;
	int			ballista_ammo;
	int			kineticore_ammo, kineticore_ammo2;
	int			iCrossbowAmmo;
	int			shockwave_ammo;
	int			sidewinder_ammo;
	int			shotcycler_ammo;
	int	iShotCycle;							// Number of shots to cycle through (shotcycler).
	//int	iGreekFireAmmo;
#elif GAME_ADAMAS
	int		iBulletAmmo;
#endif

	// Animation
	int	iAnimationCurrent;				// Current frame of current sequence.
	int animation_end;					// Last frame of current sequence.
	double animation_time;				// The speed of the animation.
	ServerEntityFrame_t *iFrames;		// Active frame group.
	int	iWeaponAnimationCurrent;		// Current frame of current sequence.
	int iWeaponAnimationEnd;			// Last frame of current sequence.
	int iWeaponIdleFrame;				// Frame to return to for "idling" after sequence.
	float fWeaponAnimationTime;			// The speed of the animation.
	ServerEntityFrame_t *iWeaponFrames;	// Active weapon frame group.

	// Misc
	char			*cInfoMessage;				// see server_point > Point_InfoMessage.
	bool			bleed;						// Do we bleed? TODO: Move this into monster struct!

	int	iDamageType;	// The type of damage this entity can recieve.
	int	maxhealth;		// An entities maximum health, they can't gain anymore than this.
	int iOldHealth;		// Previous amount of health.

	double			dDamageTime,				// Time between each amount of damage.
		steptime;					// Time between each step.
	double			pain_finished,
		air_finished,
		dAttackFinished,			// Time before we can attack again.
		dMoveFinished;

	float jump_velocity;

	float swim_flag;					// Time before next splash sound.
	int	hit;

	int	flags; // Local entity flags.

	float			spawndelay;				// Delay before next spawn.
	PLVector3D	pos1;
	PLVector3D	pos2;

	PlayerTeam_t	team;						// Current active team.
	void(*think1)(ServerEntity_t *ent, ServerEntity_t *other);
	PLVector3D	finaldest;
	char			*killtarget;
	double			delay;			// Delay before doing a task.
	ServerEntity_t	*trigger_field;
	int				iFireMode;		// Active fire mode for weapons.

	ServerEntity_t	*activator;

	const char  *oldmodel;		// Last model.
	char			cOldStyle;

	double 			laddertime;
	double 			ladderjump;
	double			zerogtime;

	// CTF states
	ServerEntity_t		*flag;				// Currently owned flag (if any).
	ServerEntity_t		*owner;			// Owner entity (we usually don't collide with this guy)

	// Door shizz

	// fixed data
	PLVector3D start_origin;
	PLVector3D start_angles;
	PLVector3D end_origin;
	PLVector3D end_angles;

	// state data
	PLVector3D		dir;

	// Angles
	PLVector3D forward;
	PLVector3D right;
	PLVector3D vUp;

	// Vehicles
	ServerEntity_t *vehicle;	// Current vehicle.
	int iVehicleSlot;			// Occupied vehicle slot.

	// Weapons
	int	iBarrelCount;			// For cycling barrel animations.

	void(*KilledFunction)(ServerEntity_t *self, ServerEntity_t *other, EntityDamageType_t type);
	void(*RespawnFunction)(ServerEntity_t *self);
	void(*DamagedFunction)(ServerEntity_t *self, ServerEntity_t *other, EntityDamageType_t type);
} ServerGameVariables_t;

//----------------------------

#define TE_EXPLOSION		3

/*
	Waypoints
*/

typedef enum
{
	WAYPOINT_TYPE_DEFAULT,	// Basic point

	WAYPOINT_TYPE_JUMP,		// Next waypoint needs a jump
	WAYPOINT_CLIMB,		// Next waypoint needs a climb
	WAYPOINT_COVER,		// Is behind cover
	WAYPOINT_ITEM,		// Has an item nearby
	WAYPOINT_WEAPON,	// Has a weapon nearby
	WAYPOINT_TYPE_INTEREST,	// Waypoint that exists purely just for points of interest.
	WAYPOINT_SPAWN,		// Near a spawn point
	WAYPOINT_SPAWNAREA,	// Near a spawn area
	WAYPOINT_TYPE_SWIM		// Underwater
} WaypointType_t;

typedef struct waypoint_s
{
	const char			*cName;		// The name for the waypoint
	unsigned int		number;		// Each point is assigned it's own number
	ServerEntity_t		*eEntity;	// The entity currently occupying that waypoint
	struct	waypoint_s	*next;		// The next point to target.
	struct	waypoint_s	*last;		// The last point we were at.
	PLVector3D			position;	// The waypoints position.
	bool				bOpen;		// Check to see if the waypoint currently is occupied.
	WaypointType_t		wType;		// Type of point (duck, jump, climb etc.)
} Waypoint_t;

/**/

#include "shared_client.h"

#ifndef KATANA	// TODO: Sort this out!!!
typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;
#endif

typedef struct
{
	PLVector3D	normal;
	float	dist;
} plane_t;

typedef struct
{
	bool	all_solid,		// If true, plane is not valid.
			bStartSolid,	// If true, the initial point was in a solid area.
			bOpen, bWater;

	float	fraction;		// time completed, 1.0 = didn't hit anything

	PLVector3D	endpos;			// final position

	plane_t	plane;			// surface normal at impact

	ServerEntity_t	*ent;			// entity the surface is on
} trace_t;

typedef struct ServerEntityBaseVariables_s
{
	// Weapons
	int			secondary_ammo, primary_ammo;
	int			iWeaponFrame;

	// Editor
	const char *classname;
	char *name;			// The specified name for the entity.
	char *noise;
	const char *model;

	int	health;

	int	modelindex;

	PLVector3D absmin;
	PLVector3D absmax;

	float		ltime;	// Local time for ents.

	int			movetype;

	PLVector3D origin;
	PLVector3D oldorigin;
	PLVector3D velocity;
	PLVector3D angles;
	PLVector3D avelocity;
	PLVector3D punchangle;

	int			frame;
	int			effects;

	PLVector3D mins, maxs;
	PLVector3D size;

	double		nextthink;

	ServerEntity_t *groundentity;

	int	iScore;
	int	iActiveWeapon;	// Current active weapon ID
	const char *cViewModel;	// The model that appears in our view, this is traditionally used for weapons.

	int	items;
	int	inventory[128];

	PLColour	vLight;

	bool takedamage;		// TODO: Make local.

	ServerEntity_t *chain;

	PLVector3D view_ofs;
	unsigned char button[3];

	int		impulse;
	bool bFixAngle;

	PLVector3D v_angle;

	float		idealpitch;

	char *netname;	// Players name on the server.

	ServerEntity_t *enemy;		// TODO: Obsolete!
	int			flags;
	float		colormap;

	double dTeleportTime;	// TODO: This is NEVER set!

	int			iArmorType, iArmorValue;

	float		waterlevel;
	int			watertype;
	float		ideal_yaw;		// TODO: Obsolete!
	int			aiment;			// TODO: What is this?? What does it do??
	int			spawnflags;
	char		*targetname;

	float		dmg_take;
	float		dmg_save;
	ServerEntity_t *seDamageInflictor;	// TODO: This is NEVER set!

	char		*message;
	float		sounds;		// TODO: This is NEVER set!

	// Physics
	PLVector3D movedir;

	void(*TouchFunction)(ServerEntity_t *seEntity, ServerEntity_t *eOther);		// Called when an entity touches another entity.
	void(*use)(ServerEntity_t *seEntity);
	void(*think)(ServerEntity_t *seEntity);
	void(*BlockedFunction)(ServerEntity_t *seEntity, ServerEntity_t *seOther);	// Called when an entity is blocked against another.
} ServerEntityBaseVariables_t;

//-----------------
// AI

typedef struct ServerEntityAIVariables_s
{
	void(*Think)(ServerEntity_t *entity);
	void(*State)(ServerEntity_t *entity);

	// Movement overrides.
	void(*Movement)(ServerEntity_t *entity);	// Overrides complete movement functionality.
	void(*Jump)(ServerEntity_t *entity);		// Called after the jump has been started; typically used for sounds / animation.
	void(*Land)(ServerEntity_t *entity);		// Called upon landing from a jump.

	Waypoint_t *target_move;

	unsigned int current_state, current_think;
	unsigned int current_movement;

	float current_movespeed;
} AIVariables_t;

//-----------------

/*	If this is changed remember
	to recompile the engine too! */
typedef struct ServerEntity_s
{
	// Shared
	bool free;

	link_t				area;
	int					num_leafs;
	short				leafnums[MAX_ENT_LEAFS];
	EntityState_t		baseline;
	unsigned char		alpha;

	bool bSendInterval;

	float fFreeTime;

	ServerEntityBaseVariables_t		v;		// Global / Base variables, shared between game and engine.
	AIVariables_t					ai;		// Specific towards AI/monsters.

	ServerEntityModel_t			Model;		// Variables that affect the model used for the entity.
	ServerEntityPhysics_t		Physics;	// Variables affecting how the entity is physically treated.
	ServerGameVariables_t		local;		// All variables specific towards the game, that aren't used by the engine.
	ServerMonsterVariables_t	Monster;	
	ServerEntityVehicle_t		Vehicle;	// Vehicle variables.
} ServerEntity_t;

#define	SERVER_ENTITY_NEXT(e)			((ServerEntity_t *)( (uint8_t *)e + sizeof(ServerEntity_t)))
#define	SERVER_ENTITY_TOHANDLE(e)		((uint8_t*)e-(uint8_t*)sv.edicts)
#define SERVER_ENTITY_FROMHANDLE(e)		((ServerEntity_t *)((uint8_t *)sv.edicts + e))
#define	SERVER_ENTITY_FIELD(y)			(intptr_t)&(((ServerEntity_t*)0)->y)

/**/

typedef struct ServerClient_s
{
	bool		active;				// FALSE = client is free
	bool		bSpawned;			// FALSE = don't send datagrams
	bool		dropasap;			// has been told to go to another level
	bool		privileged;			// can execute any host command
	bool		sendsignon;			// only valid before spawned

	double			last_message;		// reliable messages must be sent
	// periodically

	struct qsocket_s *netconnection;	// communications handle

	ClientCommand_t		cmd;				// movement
	PLVector3D		wishdir;			// intended motion calced from cmd

	sizebuf_t		message;			// can be added to at any time,
	// copied and clear once per frame
	uint8_t			msgbuf[MAX_MSGLEN];
	ServerEntity_t	*edict;				// EDICT_NUM(clientnum+1)
	char			name[32];			// for printing to other people
	int				colors;

	float			ping_times[NUM_PING_TIMES];
	int				num_pings;			// ping_times[num_pings%NUM_PING_TIMES]

	// spawn parms are carried from level to level
	float			spawn_parms[NUM_SPAWN_PARMS];

	// client known data for deltas
	int				old_frags;
} ServerClient_t;

typedef struct
{
	unsigned int			maxclients;
	int						maxclientslimit;
	struct ServerClient_s	*clients;			// [maxclients]
	int						serverflags;		// episode completion information
	bool					bChangingLevel;		// cleared when at SV_SpawnServer
} ServerStatic_t;
