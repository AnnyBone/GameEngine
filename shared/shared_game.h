/*  Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __SHAREDGAME__
#define __SHAREDGAME__

#ifndef KATANA
#include "platform.h"
#include "platform_math.h"

typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;
#endif

#include "SharedMaterial.h"

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
} DamageType_t;

typedef struct ServerEntity_s ServerEntity_t;

#define	edict_s ServerEntity_s

typedef struct
{
	ServerEntity_t *eOther;

	int		self;
	int		world;

	int		iKilledMonsters;
	float	time;
	float	force_retouch;
	char	*mapname;
	float	deathmatch;
	float	coop;
	float	teamplay;
	float	serverflags;
	float	total_secrets;
	float	total_monsters;
	float	found_secrets;

	float	parm1;
	float	parm2;
	float	parm3;
	float	parm4;
	float	parm5;
	float	parm6;
	float	parm7;
	float	parm8;
	float	parm9;
	float	parm10;
	float	parm11;
	float	parm12;
	float	parm13;
	float	parm14;
	float	parm15;
	float	parm16;
	float	parm17;
	float	parm18;
	float	parm20;
	float	parm21;
	float	parm22;
	float	parm23;
	float	parm24;
	float	parm25;
	float	parm26;
	float	parm27;
	float	parm28;
	float	parm29;
} GlobalState_t;

#define	ENTITY_MAX_INVENTORY	128

typedef struct
{
	// Weapons
	int			iSecondaryAmmo,iPrimaryAmmo;
	int			iWeaponFrame;

	// Editor
	char		*cClassname,	// The classname of the entity.
				*cName;			// The specified name for the entity.
	char		*noise;
	char		*model;
	int			iHealth;

	// [30/1/2013] Changed from float to int ~hogsy
	int			modelindex;

	MathVector3f_t absmin;
	MathVector3f_t absmax;

	float		ltime;	// Local time for ents.

	// [20/7/2012] Changed to an integer ~hogsy
	int			movetype;

	MathVector3f_t origin;
	MathVector3f_t oldorigin;
	MathVector3f_t velocity;
	MathVector3f_t angles;
	MathVector3f_t avelocity;
	MathVector3f_t punchangle;

	// [20/10/2013] Changed from a float to an integer ~hogsy
	int			frame;
	int			effects;

	MathVector3f_t mins, maxs;
	MathVector3f_t size;

	double		dNextThink;

	ServerEntity_t *groundentity;

	// [21/10/2012] Changed to an integer ~hogsy
	int			iScore;
	int			iActiveWeapon;	// Current active weapon ID
	char		*cViewModel;	// The model that appears in our view, this is traditionally used for weapons.

	int			items;
	int			iInventory[ENTITY_MAX_INVENTORY];

	MathVector4f_t	vLight;

	bool		bTakeDamage;
	ServerEntity_t *chain;

	MathVector3f_t view_ofs;
	MathVector3f_t button;

	int		impulse;
	bool bFixAngle;

	MathVector3f_t v_angle;

	float		idealpitch;
	char		*netname;
	ServerEntity_t *enemy;     // Obsolete
	int			flags;
	float		colormap;
	float		team;

	double dTeleportTime;

	int			iArmorType,iArmorValue;

	float		waterlevel;
	int			watertype;
	float		ideal_yaw;
	float		yaw_speed;
	int			aiment;
	// [9/7/2012] Changed from float to an integer ~hogsy
	int			spawnflags;
	char		*targetname;

	float		dmg_take;
	float		dmg_save;
	ServerEntity_t *eDamageInflictor;

	char		*message;
	float		sounds;

	// Physics
	MathVector3f_t movedir;

	void(*TouchFunction)(ServerEntity_t *eEntity, ServerEntity_t *eOther);		// TODO: Move into local struct.
	void(*use)(ServerEntity_t *ent);
	void(*think)(ServerEntity_t *ent);
	void(*BlockedFunction)(ServerEntity_t *seEntity, ServerEntity_t *seOther);	// Used in some places by engine_physics.
	void(*DamagedFunction)(ServerEntity_t *seEntity, ServerEntity_t *seOther);
} GlobalVariables_t;

// [12/1/2013] Model specific variables ~hogsy
typedef struct
{
	/*	Move the following here...
	frame
	*/

	float fScale;

	int	iSkin;
} ModelVariables_t;

typedef enum
{
	SOLID_NOT,		// Entity isn't solid.
	SOLID_TRIGGER,	// Entity will cause a trigger function.
	SOLID_BBOX,		// Entity is solid.
	SOLID_SLIDEBOX,	// Entity is solid and moves.
	SOLID_BSP
} PhysicsSolidTypes_t;

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

	float	fMass,		// Sets the mass of the entity.
			fGravity,	// Sets the gravity which is enforced on the entity.
			fFriction;	// Sets the amount of friction that effects this entity and others.

	PhysicsSolidTypes_t	iSolid;	// Sets the collision/solid type for the entity.

	ServerEntity_t	*eIgnore;	// Tells the entity to ignore collisions with this entity.
} PhysicsVariables_t;

typedef struct
{
	int	iEmotion,
		iPriority;

	double dResetDelay;
} MonsterEmotion_t;

// Monster specific variables
typedef struct
{
	// [15/7/2012] Position we'll be moving to ~hogsy
	MathVector3f_t vTarget;
	MathVector3f_t vOldTarget;

	int		iType;

	// Think functions (obsolete)
//	void	(*think_stand)(ServerEntity_t *ent);
//	void	(*think_run)(ServerEntity_t *ent);
//	void	(*think_attack)(ServerEntity_t *ent);
	void(*think_pain)(ServerEntity_t *ent, ServerEntity_t *other);

	void(*Think)(ServerEntity_t *eMonster);	// Called per-frame for the specific handling of states for each monster.

	// State Functions
	void(*Idle)(ServerEntity_t *eMonster);
	void(*Jump)(ServerEntity_t *eMonster);

	// Current thought state
	int	iState,					// Primary physical state.
		iNewState,				// The new state that the entity wishes to set.
		iThink,					// Primary thought state.
		iNewThink;				// New think that the entity wishes to set.
	int	iCommandList[64];		// List of sub-commands for the monster to execute.

	//MonsterEmotion_t	meEmotion[11];			// Current emotion states.

	float fViewDistance;	// Distance in which a monster can detect a target within.

	// Targets
	ServerEntity_t *eEnemy;		// Current enemy.
	ServerEntity_t *eOldEnemy;	// Last enemy.
	ServerEntity_t *eFriend;	// Current friend.
	ServerEntity_t *eOldFriend;	// Last friend.
	ServerEntity_t *eTarget;	// Current target.
	ServerEntity_t *eOldTarget;	// Last target.
} MonsterVariables_t;

/*	Variables used for vehicles.
*/
typedef struct
{
	int		iPassengers,									// Current number of passengers.
			iMaxPassengers;									// Maximum passengers the vehicle can carry.
	int		iSlot[16];
	int		iFuel,											// Current amount of fuel in the vehicle.
			iMaxFuel;										// Maximum amount of fuel that can be in the vehicle.
	int		iType;											// Type of vehicle.

	float	fMaxSpeed;										// Maximum velocity that the vehicle can travel.

	bool	bActive;										// Is the vehicle turned on or not?

	void(*Enter)(ServerEntity_t *eVehicle, ServerEntity_t *eOther);	// Function to call when a player enters the vehicle.
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
	void	(*Function)(ServerEntity_t *eEntity);

	int		iFrame;

	float	fSpeed;

	bool	bIsEnd;
} EntityFrame_t;

// Misc local variables
typedef struct
{
	// mapping features
	char		*sound;
	float		speed;
	int			iDamage,	// Amount of damage to inflict.
				iValue;
	int			volume;		// Volume level for sound.
	float		lip;
	int			distance;
	int			style;
	// [21/2/2014] Changed to a double and renamed ~hogsy
	double		dWait;
	int			count;
	char		*cTarget1,		// First target.
				*cTarget2;		// Second target.

	char		*cSoundStart,
				*cSoundStop,
				*cSoundMoving,
				*cSoundReturn;

#ifdef GAME_OPENKATANA
	// Powerups
	double		power_finished,power_time,		// Power Boost.
				speed_finished,speed_time,		// Speed Boost.
				acro_finished,acro_time,		// Acro Boost.
				attackb_finished,attackb_time,	// Attack Boost.
				vita_finished,vita_time;		// Vita Boost.

	// Ammo
	int			iC4Ammo,
				claw_ammo;
	int			glock_ammo,glock_ammo2;
	int			ionblaster_ammo;
	int			discus_ammo,discus_ammo2;
	int			pulserifle_ammo;
	int			hermes_ammo;
	int			cordite_ammo;
	int			barrier_ammo;
	int			ballista_ammo;
	int			kineticore_ammo,kineticore_ammo2;
	int			iCrossbowAmmo;
	int			shockwave_ammo;
	int			sidewinder_ammo;
	int			shotcycler_ammo;
	int			iShotCycle;							// Number of shots to cycle through (shotcycler).
	int			iGreekFireAmmo;
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
	bool			bBleed;						// Do we bleed?

	int	iDamageType;	// The type of damage this entity can recieve.
	int	iMaxHealth;		// An entities maximum health, they can't gain anymore than this.
	int iOldHealth;		// Previous amount of health.

	double			dDamageTime,				// Time between each amount of damage.
					dStepTime;					// Time between each step.
	double			dPainFinished,
					dAirFinished,
					dAttackFinished,			// Time before we can attack again.
					dMoveFinished;

	float fJumpVelocity;

	float			swim_flag;					// Time before next splash sound.
	int				hit;
	int				state;						// Main state.
	float			fSpawnDelay;				// Delay before next spawn.
	MathVector3f_t	pos1;
	MathVector3f_t	pos2;
	char			*deathtype;
	PlayerTeam_t	pTeam;						// Current active team.
	void(*think1)(ServerEntity_t *ent, ServerEntity_t *other);
	MathVector3f_t	finaldest;
	char			*killtarget;
	float			delay;			// Delay before doing a task.
	ServerEntity_t			*trigger_field;
	int				iFireMode;		// Active fire mode for weapons.
	ServerEntity_t			*bomb,
					*activator;
	char			*cOldModel;		// Last model.
	char			cOldStyle;

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

	int iDelta[2][2];	// X & Y Delta.

	// Vehicles
	ServerEntity_t *eVehicle;	// Current vehicle.
	int iVehicleSlot;			// Occupied vehicle slot.

	// Weapons
	int	iBarrelCount;			// For cycling barrel animations.

	void(*KilledFunction)(ServerEntity_t *seEntity, ServerEntity_t *seOther);
} GameVariables_t;

//----------------------------

#define TE_EXPLOSION		3

#include "shared_client.h"

typedef struct
{
	MathVector3f_t	normal;
	float	dist;
} plane_t;

typedef struct
{
	bool	bAllSolid,		// If true, plane is not valid.
			bStartSolid,	// If true, the initial point was in a solid area.
			bOpen,bWater;

	float	fraction;		// time completed, 1.0 = didn't hit anything
	MathVector3f_t	endpos;			// final position
	plane_t	plane;			// surface normal at impact
	ServerEntity_t	*ent;			// entity the surface is on
} trace_t;

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

	GlobalVariables_t	v;

	ModelVariables_t	Model;		// Variables that affect the model used for the entity.
	PhysicsVariables_t	Physics;	// Variables affecting how the entity is physically treated.
	GameVariables_t		local;		// All variables specific towards the game, that aren't used by the engine.
	MonsterVariables_t	Monster;	// Specific towards AI/monsters.
	VehicleVariables_t	Vehicle;	// Vehicle variables.
} ServerEntity_t;

#define	NEXT_EDICT(e) ((ServerEntity_t *)( (uint8_t *)e + sizeof(ServerEntity_t)))

#define	ServerEntity_tO_PROG(e) ((uint8_t*)e-(uint8_t*)sv.edicts)
#define PROG_TO_EDICT(e) ((ServerEntity_t *)((uint8_t *)sv.edicts + e))

#define	FIELD(y)	(intptr_t)&(((ServerEntity_t*)0)->y)

// TODO: Replace with EngineExport_t!
typedef struct
{
	// Server
	int(*Server_PointContents)(MathVector3f_t point);
	int(*Server_GetNumEdicts)(void);

	void(*Server_MakeStatic)(ServerEntity_t *ent);
	void(*Server_BroadcastPrint)(char *fmt, ...);														// Sends a message to all clients.
	void(*Server_SinglePrint)(ServerEntity_t *eEntity, char *cMessage);											// Sends a message to a specified client.
	void(*Server_PrecacheResource)(int iType, const char *ccResource);									// Precaches the specified resource.
	void(*Server_Restart)(void);																		// Restarts the server.
	void(*Server_ChangeLevel)(const char *ccNewLevel);													// Changes the level.
	void(*Server_AmbientSound)(MathVectorf_t *vPosition, const char *cPath, int iVolume, int iAttenuation);		// Plays an ambient sound (a constant sound) from the given location.

	trace_t(*Server_Move)(MathVector3f_t start, MathVector3f_t mins, MathVector3f_t maxs, MathVector3f_t end, int type, ServerEntity_t *passedict);
	
	ServerEntity_t*(*Server_FindRadius)(MathVector3f_t origin, float radius);												// Finds entities within a specific radius.
	ServerEntity_t*(*Server_FindEntity)(ServerEntity_t *eStartEntity, char *cName, bool bClassname);						// Finds a specified entity either by classname or by entity name.
	ServerEntity_t*(*Server_GetEdicts)(void);

	char		*(*Server_GetLevelName)(void);	// Returns the name of the currently active level.
	
	double		(*Server_GetFrameTime)(void);	// Returns host time.

	// Client
	int(*Client_GetEffect)(const char *cPath);					// Get an effect index.
	int(*Client_GetStat)(ClientStat_t csStat);					// Get a client statistic (health etc.)
	
	void(*Client_PrecacheResource)(int iType, char *cResource);	// Precache a resource client-side.
	void(*Client_SetMenuCanvas)(int iCanvas);					// Set the canvas type that the menu will use.
	void(*Client_AddMenuState)(int iState);						// Adds a new state to the clients menu.
	void(*Client_RemoveMenuState)(int iState);					// Removes a state from the clients menu.
	
	ClientEntity_t		*(*Client_GetViewEntity)(void);							// Returns the entity representing the players view model.
	ClientEntity_t		*(*Client_GetPlayerEntity)(void);						// Returns the entity representing the player.
	
	DynamicLight_t	*(*Client_AllocateDlight)(int key);						// Allocate a new dynamic light.
	
	Particle_t		*(*Client_AllocateParticle)(void);						// Allocate a new particle effect.

	// Global
	MathVectorf_t	*(*GetLightSample)(MathVector3f_t vOrigin);	// Gets the current lightmap sample for the specified entity.

	bool(*Material_Precache)(const char *ccPath);

	// Pre 9/4/2012 (Update all these)
	void(*Con_Printf)(char *fmt, ...);	// Appears to client in console. Standard message.
	void(*Con_DPrintf)(char *fmt, ...);	// Only appears if launched/running in developer mode.
	void(*Con_Warning)(char *fmt, ...);	// Highlighted message to indicate an issue.

	void(*SetMessageEntity)(ServerEntity_t *ent);
	void(*CenterPrint)(ServerEntity_t *ent, char *msg);	// Sends a message to the specified client and displays the message at the center of the screen.
	void(*Sys_Error)(char *error, ...);
	void(*SetModel)(ServerEntity_t *ent, char *m);		// Sets the model for the specified entity.
	void(*Particle)(float org[3], float dir[3], float scale, char *texture, int count);
	void(*Flare)(MathVector3f_t org, float r, float g, float b, float a, float scale, char *texture);
	void(*Sound)(ServerEntity_t *ent, int channel, char *sample, int volume, float attenuation);
	void(*LinkEntity)(ServerEntity_t *ent, bool touch_triggers);
	void(*UnlinkEntity)(ServerEntity_t *ent);
	void(*FreeEntity)(ServerEntity_t *ed);

	Material_t*(*LoadMaterial)(const char *cPath);

	// Draw Functions
	void(*DrawPic)(char *path, float alpha, int x, int y, int w, int h);
	void(*DrawString)(int x, int y, char *msg);
	void(*DrawFill)(int x, int y, int w, int h, float r, float g, float b, float alpha);
	void(*DrawMaterialSurface)(Material_t *mMaterial, int iSkin, int x, int y, int w, int h, float fAlpha);

	void(*Cvar_RegisterVariable)(cvar_t *variable, void(*Function)(void));
	void(*Cvar_SetValue)(char *var_name, float value);
	void(*LightStyle)(int style, char *val);
	void(*Cmd_AddCommand)(char *cmd_name, void(*function)(void));
	void(*WriteByte)(int mode, int command);
	void(*WriteCoord)(int mode, float f);
	void(*WriteAngle)(int mode, float f);
	void(*WriteEntity)(int mode, ServerEntity_t *ent);
	void(*ShowCursor)(bool bShow);

	int(*ReadByte)(void);
	float(*ReadCoord)(void);
	int(*GetScreenWidth)(void);	// Returns the active screen width.
	int(*GetScreenHeight)(void);	// Returns the active screen height.

	void(*GetCursorPosition)(int *X, int *Y);

	ServerEntity_t	*(*Spawn)(void);
} ModuleImport_t;

typedef struct
{
	int		iVersion;

	void	(*Initialize)(void);
	void	(*Shutdown)(void);
	void	(*Draw)(void);			// Called during video processing.

	//	Game
	char	*Name;																						// Name of the currently active game (used as the name for the window).
	bool(*Game_Init)(int state, ServerEntity_t *ent, double dTime);											// For both server-side and client-side entry
	void(*ChangeYaw)(ServerEntity_t *ent);
	void(*SetSize)(ServerEntity_t *ent, float mina, float minb, float minc, float maxa, float maxb, float maxc);	// Sets the size of an entity.

	void(*Client_Initialize)(void);
	void(*Client_RelinkEntities)(ClientEntity_t *ent, int i, double dTime);
	void(*Client_ParseTemporaryEntity)(void);
	void(*Client_ViewFrame)(void);																	// Called per-frame to handle players view.

	void(*Server_Initialize)(void);																	// Initializes the server.
	void(*Server_EntityFrame)(ServerEntity_t *eEntity);
	void(*Server_KillClient)(ServerEntity_t *eClient);														// Tells the specified client to die.
	void(*Server_SetSizeVector)(ServerEntity_t *eEntity, MathVector3f_t vMin, MathVector3f_t vMax);				// Set the size of an entity by vector.
	void(*Server_SpawnPlayer)(ServerEntity_t *ePlayer);													// Spawns the player (SERVER_PUTCLIENTINSERVER).
	void(*Server_StartFrame)(void);																	// Called at the start of each physics frame.
	bool(*Server_SpawnEntity)(ServerEntity_t *ent);														// Puts a specific entity into the server.

	void(*Physics_SetGravity)(ServerEntity_t *eEntity);			// Sets the current gravity for the given entity.
	void(*Physics_CheckWaterTransition)(ServerEntity_t *eEntity);
	void(*Physics_CheckVelocity)(ServerEntity_t *ent);				// Checks the velocity of physically simulated entities.
	void(*Physics_WallFriction)(ServerEntity_t *eEntity, trace_t *trLine);
	void(*Physics_Impact)(ServerEntity_t *eEntity, ServerEntity_t *eOther);

	trace_t(*Physics_PushEntity)(ServerEntity_t *eEntity, MathVector3f_t mvPush);

	bool(*Physics_CheckWater)(ServerEntity_t *eEntity);
} GameExport_t;

#define GAME_VERSION (sizeof(GameExport_t)+sizeof(ModuleImport_t)+sizeof(GlobalVariables_t*))	// Version check that's used for Menu and Launcher.

#endif
