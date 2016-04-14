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

typedef struct
{
	/*	Move the following here...
	frame
	*/

	float fScale;

	int	iSkin;
} ServerModelVariables_t;

typedef enum
{
	SOLID_NOT,		// Entity isn't solid.
	SOLID_TRIGGER,	// Entity will cause a trigger function.
	SOLID_BBOX,		// Entity is solid.
	SOLID_SLIDEBOX,	// Entity is solid and moves.
	SOLID_BSP
} ServerSolidType_t;

typedef MaterialProperty_t ServerSurfaceType_t;

/*	Different type's of damage.
*/
typedef enum
{
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_EXPLODE,
	DAMAGE_TYPE_BURN,
	DAMAGE_TYPE_FREEZE,
	DAMAGE_TYPE_GRAVITY,
	DAMAGE_TYPE_CRUSH,
	DAMAGE_TYPE_FALL,

	DAMAGE_TYPE_NONE
} ServerDamageType_t;

typedef struct ServerEntity_s ServerEntity_t;

typedef struct
{
	/*	Move the following here...
	movetype
	watertype
	waterlevel
	velocity
	avelocity
	solid
	*/

	float fMass;		// Sets the mass of the entity.
	float fGravity;		// Sets the gravity which is enforced on the entity.
	float fFriction;	// Sets the amount of friction that effects this entity and others.

	ServerSolidType_t		iSolid;		// Sets the collision/solid type for the entity.
	ServerSurfaceType_t		property;	// Type of surface, e.g. wood, rock etc

	ServerEntity_t	*eIgnore;	// Tells the entity to ignore collisions with this entity.
} ServerPhysicsVariables_t;

// Monster specific variables
typedef struct
{
	int	iType;

	void(*Frame)(ServerEntity_t *entity);	// Called per-frame for the specific handling of states for each monster.

	// State Functions
	void(*Idle)(ServerEntity_t *entity);
	void(*Jump)(ServerEntity_t *entity);
	void(*Land)(ServerEntity_t *entity);
	void(*Pain)(ServerEntity_t *entity, ServerEntity_t *other, ServerDamageType_t type);

	// Current thought state
	int	state;					// Current physical state.
	int	think;					// Current thought process.
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

/*	Variables used for vehicles.
*/
typedef struct
{
	int		iPassengers,									// Current number of passengers.
		iMaxPassengers;										// Maximum passengers the vehicle can carry.
	int		iSlot[4];
	int		iFuel,											// Current amount of fuel in the vehicle.
		iMaxFuel;											// Maximum amount of fuel that can be in the vehicle.
	int		iType;											// Type of vehicle.

	float	fMaxSpeed;										// Maximum velocity that the vehicle can travel.

	bool	bActive;										// Is the vehicle turned on or not?

	void(*Enter)(ServerEntity_t *eVehicle, ServerEntity_t *eOther);		// Function to call when a player enters the vehicle.
	void(*Exit)(ServerEntity_t *eVehicle, ServerEntity_t *eOther);		// Function to call when a player leaves the vehicle.
	void(*Kill)(ServerEntity_t *eVehicle, ServerEntity_t *eOther);		// Function to call when the vehicle is destroyed.
} VehicleVariables_t;

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

typedef struct
{
	void(*Function)(ServerEntity_t *eEntity);

	int	iFrame;

	float fSpeed;	// TODO: This is time-based... Change to double instead?

	bool bIsEnd;
} EntityFrame_t;

// Misc local variables
typedef struct
{
	// TODO: Move all mapping parameters to its own struct.
	char		*sound;
	float		speed;

	int	iDamage,	// Amount of damage to inflict.
		iValue;		// TODO: Make this a little less... Ambiguous?

	int			volume;		// Volume level for sound.
	float		lip;
	int			distance;
	int			style;

	double dWait;

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
	double		power_finished, power_time,		// Power Boost.
		speed_finished, speed_time,		// Speed Boost.
		acro_finished, acro_time,		// Acro Boost.
		attackb_finished, attackb_time,	// Attack Boost.
		vita_finished, vita_time;		// Vita Boost.

	// Ammo
	int			iC4Ammo,
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
	int	iAnimationCurrent;			// Current frame of current sequence.
	int iAnimationEnd;				// Last frame of current sequence.
	double dAnimationTime;			// The speed of the animation.
	EntityFrame_t *iFrames;			// Active frame group.
	int	iWeaponAnimationCurrent;	// Current frame of current sequence.
	int iWeaponAnimationEnd;		// Last frame of current sequence.
	int iWeaponIdleFrame;			// Frame to return to for "idling" after sequence.
	float fWeaponAnimationTime;		// The speed of the animation.
	EntityFrame_t *iWeaponFrames;	// Active weapon frame group.

	// Misc
	char			*cInfoMessage;				// see server_point > Point_InfoMessage.
	bool			bBleed;						// Do we bleed? TODO: Move this into monster struct!

	int	iDamageType;	// The type of damage this entity can recieve.
	int	iMaxHealth;		// An entities maximum health, they can't gain anymore than this.
	int iOldHealth;		// Previous amount of health.

	double			dDamageTime,				// Time between each amount of damage.
		dStepTime;					// Time between each step.
	double			dPainFinished,
		dAirFinished,
		dAttackFinished,			// Time before we can attack again.
		dMoveFinished;

	float jump_velocity;

	float swim_flag;					// Time before next splash sound.
	int	hit;

	int	iLocalFlags; // Local entity flags.

	float			fSpawnDelay;				// Delay before next spawn.
	MathVector3f_t	pos1;
	MathVector3f_t	pos2;
	char			*deathtype;
	PlayerTeam_t	pTeam;						// Current active team.
	void(*think1)(ServerEntity_t *ent, ServerEntity_t *other);
	MathVector3f_t	finaldest;
	char			*killtarget;
	double			delay;			// Delay before doing a task.
	ServerEntity_t	*trigger_field;
	int				iFireMode;		// Active fire mode for weapons.

	ServerEntity_t	*activator;

	char			*cOldModel;		// Last model.
	char			cOldStyle;

	double 			dLadderTime;
	double 			dLadderJump;
	double			dZeroGTime;

	// CTF states
	ServerEntity_t		*flag;				// Currently owned flag (if any).
	ServerEntity_t		*eOwner;			// Owner entity (we usually don't collide with this guy)

	// Door shizz

	// fixed data
	MathVector3f_t start_origin;
	MathVector3f_t start_angles;
	MathVector3f_t end_origin;
	MathVector3f_t end_angles;

	// state data
	MathVector3f_t		dir;

	// Angles
	MathVector3f_t vForward;
	MathVector3f_t vRight;
	MathVector3f_t vUp;

	// Vehicles
	ServerEntity_t *eVehicle;	// Current vehicle.
	int iVehicleSlot;			// Occupied vehicle slot.

	// Weapons
	int	iBarrelCount;			// For cycling barrel animations.

	void(*KilledFunction)(ServerEntity_t *self, ServerEntity_t *other, ServerDamageType_t type);
	void(*RespawnFunction)(ServerEntity_t *self);
	void(*DamagedFunction)(ServerEntity_t *self, ServerEntity_t *other, ServerDamageType_t type);
} ServerGameVariables_t;

//----------------------------

#define TE_EXPLOSION		3

#include "shared_client.h"

#ifndef KATANA	// TODO: Sort this out!!!
typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;
#endif

typedef struct
{
	MathVector3f_t	normal;
	float	dist;
} plane_t;

typedef struct
{
	bool	bAllSolid,		// If true, plane is not valid.
			bStartSolid,	// If true, the initial point was in a solid area.
			bOpen, bWater;

	float	fraction;		// time completed, 1.0 = didn't hit anything

	MathVector3f_t	endpos;			// final position

	plane_t	plane;			// surface normal at impact

	ServerEntity_t	*ent;			// entity the surface is on
} trace_t;

typedef struct GlobalVariables_e
{
	// Weapons
	int			iSecondaryAmmo, iPrimaryAmmo;
	int			iWeaponFrame;

	// Editor
	char
		*cClassname,	// The classname of the entity.
		*cName;			// The specified name for the entity.
	char *noise;
	char *model;

	int	iHealth;

	int	modelindex;

	MathVector3f_t absmin;
	MathVector3f_t absmax;

	float		ltime;	// Local time for ents.

	int			movetype;

	MathVector3f_t origin;
	MathVector3f_t oldorigin;
	MathVector3f_t velocity;
	MathVector3f_t angles;
	MathVector3f_t avelocity;
	MathVector3f_t punchangle;

	int			frame;
	int			effects;

	MathVector3f_t mins, maxs;
	MathVector3f_t size;

	double		dNextThink;

	ServerEntity_t *groundentity;

	int	iScore;
	int	iActiveWeapon;	// Current active weapon ID
	char *cViewModel;	// The model that appears in our view, this is traditionally used for weapons.

	int	items;
	int	iInventory[128];

	MathVector4f_t	vLight;

	bool bTakeDamage;		// TODO: Make local.

	ServerEntity_t *chain;

	MathVector3f_t view_ofs;
	MathVector3f_t button;

	int		impulse;
	bool bFixAngle;

	MathVector3f_t v_angle;

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
	MathVector3f_t movedir;

	void(*TouchFunction)(ServerEntity_t *seEntity, ServerEntity_t *eOther);		// Called when an entity touches another entity.
	void(*use)(ServerEntity_t *seEntity);
	void(*think)(ServerEntity_t *seEntity);
	void(*BlockedFunction)(ServerEntity_t *seEntity, ServerEntity_t *seOther);	// Called when an entity is blocked against another.
} ServerBaseVariables_t;

/*	If this is changed remember
to recompile the engine too!    */
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

	ServerBaseVariables_t	v;

	ServerModelVariables_t		Model;		// Variables that affect the model used for the entity.
	ServerPhysicsVariables_t	Physics;	// Variables affecting how the entity is physically treated.
	ServerGameVariables_t		local;		// All variables specific towards the game, that aren't used by the engine.
	ServerMonsterVariables_t	Monster;	// Specific towards AI/monsters.
	VehicleVariables_t			Vehicle;	// Vehicle variables.
} ServerEntity_t;

#define	NEXT_EDICT(e)			((ServerEntity_t *)( (uint8_t *)e + sizeof(ServerEntity_t)))
#define	ServerEntity_tO_PROG(e) ((uint8_t*)e-(uint8_t*)sv.edicts)
#define PROG_TO_EDICT(e)		((ServerEntity_t *)((uint8_t *)sv.edicts + e))
#define	ENTITY_FIELD(y)			(intptr_t)&(((ServerEntity_t*)0)->y)

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
	MathVector3f_t		wishdir;			// intended motion calced from cmd

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
