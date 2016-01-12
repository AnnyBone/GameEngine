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

#ifndef SHARED_GAME_H
#define SHARED_GAME_H

#include "shared_common.h"
#include "shared_material.h"
#include "shared_server.h"

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
	const	char		*cName;		// The name for the waypoint
	int					number;		// Each point is assigned it's own number
	ServerEntity_t		*eEntity;	// The entity currently occupying that waypoint
#ifdef DEBUG_WAYPOINT
	ServerEntity_t		*eDebug;	// Entity to use for displaying waypoint position
#endif
	struct	waypoint_s	*next;		// The next point to target.
	struct	waypoint_s	*last;		// The last point we were at.
	vec3_t				position;	// The waypoints position.
	bool				bOpen;		// Check to see if the waypoint currently is occupied.
	WaypointType_t		wType;		// Type of point (duck, jump, climb etc.)
} Waypoint_t;

/**/

typedef struct
{
	int		world;

	int		iKilledMonsters;
	float	time;
	float	force_retouch;		// TODO: This isn't setup properly!
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
	void(*Client_SetMenuCanvas)(VideoCanvasType_t Canvas);		// Set the canvas type that the menu will use.
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
	void(*Con_Printf)(const char *fmt, ...);	// Appears to client in console. Standard message.
	void(*Con_DPrintf)(const char *fmt, ...);	// Only appears if launched/running in developer mode.
	void(*Con_Warning)(const char *fmt, ...);	// Highlighted message to indicate an issue.

	void(*SetMessageEntity)(ServerEntity_t *ent);
	void(*CenterPrint)(ServerEntity_t *ent, char *msg);	// Sends a message to the specified client and displays the message at the center of the screen.
	void(*Sys_Error)(const char *error, ...);
	void(*SetModel)(ServerEntity_t *ent, char *m);		// Sets the model for the specified entity.
	void(*Particle)(float org[3], float dir[3], float scale, char *texture, int count);
	void(*Flare)(MathVector3f_t org, float r, float g, float b, float a, float scale, char *texture);
	void(*Sound)(ServerEntity_t *ent, int channel, char *sample, int volume, float attenuation);
	void(*LinkEntity)(ServerEntity_t *ent, bool touch_triggers);
	void(*UnlinkEntity)(ServerEntity_t *ent);
	void(*FreeEntity)(ServerEntity_t *ed);

	Material_t*(*LoadMaterial)(const char *cPath);

	// Memory
	void*(*Hunk_AllocateName)(int size, char *name);
	void*(*Hunk_Alloc)(int size);

	// Draw Functions
	void(*DrawString)(int x, int y, char *msg);
	void(*DrawRectangle)(int x, int y, int w, int h, Colour_t colour);
	void(*DrawMaterialSurface)(Material_t *mMaterial, int iSkin, int x, int y, int w, int h, float fAlpha);

	void(*Cvar_RegisterVariable)(ConsoleVariable_t *variable, void(*Function)(void));
	void(*Cvar_SetValue)(const char *var_name, float value);
	void(*LightStyle)(int style, char *val);
	void(*Cmd_AddCommand)(char *cmd_name, void(*function)(void));
	void(*WriteByte)(int mode, int command);
	void(*WriteCoord)(int mode, float f);
	void(*WriteAngle)(int mode, float f);
	void(*WriteEntity)(int mode, ServerEntity_t *ent);

	void(*MSG_WriteByte)(sizebuf_t *sb, int c);
	void(*MSG_WriteChar)(sizebuf_t *sb, int c);
	void(*MSG_WriteShort)(sizebuf_t *sb, int c);
	void(*MSG_WriteLong)(sizebuf_t *sb, int c);
	void(*MSG_WriteFloat)(sizebuf_t *sb, float f);
	void(*MSG_WriteString)(sizebuf_t *sb, const char *s);
	void(*MSG_WriteCoord)(sizebuf_t *sb, float f);
	void(*MSG_WriteAngle)(sizebuf_t *sb, float f);

	void(*ShowCursor)(bool bShow);

	int(*ReadByte)();
	float(*ReadCoord)();
	float(*ReadFloat)();

	int(*GetScreenWidth)(void);	// Returns the active screen width.
	int(*GetScreenHeight)(void);	// Returns the active screen height.

	void(*GetCursorPosition)(int *X, int *Y);

	ServerEntity_t	*(*Spawn)(void);
} ModuleImport_t;

typedef struct
{
	int	iVersion;

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
	void(*Server_KillClient)(ServerEntity_t *eClient);												// Tells the specified client to die.
	void(*Server_SetSizeVector)(ServerEntity_t *eEntity, MathVector3f_t vMin, MathVector3f_t vMax);	// Set the size of an entity by vector.
	void(*Server_SpawnPlayer)(ServerEntity_t *ePlayer);												// Spawns the player (SERVER_PUTCLIENTINSERVER).
	void(*Server_PreFrame)(void);																	// Called at the start of each physics frame.
	void(*Server_PostFrame)(void);																	// Called at the end of each physics frame.
	void(*Server_SendClientInformation)(ServerClient_t *client);
	void(*Server_ParseEntityField)(char *key, char *value, ServerEntity_t *entity);					// Parse the given field and value into an entities struct.
	bool(*Server_SpawnEntity)(ServerEntity_t *ent);													// Puts a specific entity into the server.

	void(*Physics_SetGravity)(ServerEntity_t *eEntity);			// Sets the current gravity for the given entity.
	void(*Physics_CheckWaterTransition)(ServerEntity_t *eEntity);
	void(*Physics_CheckVelocity)(ServerEntity_t *ent);				// Checks the velocity of physically simulated entities.
	void(*Physics_WallFriction)(ServerEntity_t *eEntity, trace_t *trLine);
	void(*Physics_Impact)(ServerEntity_t *eEntity, ServerEntity_t *eOther);

	trace_t(*Physics_PushEntity)(ServerEntity_t *eEntity, MathVector3f_t mvPush);

	bool(*Physics_CheckWater)(ServerEntity_t *eEntity);
} GameExport_t;

#define GAME_VERSION (sizeof(GameExport_t)+sizeof(ModuleImport_t)+sizeof(GlobalVariables_t*))	// Version check that's used for Menu and Launcher.

#endif // !SHARED_GAME_H
