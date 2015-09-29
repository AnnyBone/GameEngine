/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __SERVERMAIN__
#define __SERVERMAIN__

#include "game_main.h"

#include "shared_game.h"
#include "shared_server.h"

// Base includes
#ifdef __cplusplus
#include "Entity.h"
#endif

// Legacy base includes
#include "server_physics.h"
#include "server_mode.h"
#include "server_monster.h"
#include "server_misc.h"

extern	ConsoleVariable_t	cvServerSkill;			// The difficulty level.
extern	ConsoleVariable_t	cvServerSelfDamage;		// If the player can directly kill themselves or not.
extern	ConsoleVariable_t	cvServerMaxHealth;		// The maximum amount of health that a player can have.
extern	ConsoleVariable_t	cvServerMonsters;		// Should monsters be allowed?
extern	ConsoleVariable_t	cvServerMaxScore;		// Maximum score before a round ends.
extern	ConsoleVariable_t	cvServerGameMode;		// The gamemode being used for the server.
extern	ConsoleVariable_t	cvServerPlayerModel;	// The default player model for clients joining the server.
extern	ConsoleVariable_t	cvServerRespawnDelay;	// The delay before respawning.
extern	ConsoleVariable_t	cvServerWaypointSpawn;	// Should waypoints spawn?
extern	ConsoleVariable_t	cvServerBots;			// Shoulds bots spawn?
extern	ConsoleVariable_t	cvServerDefaultHealth;	// Default amount of health for a player.
extern	ConsoleVariable_t	cvServerWaypointDelay;	// Delay between each check before spawning another waypoint.
extern	ConsoleVariable_t	cvServerWaypointParse;	// File to parse for waypoint positions.
extern	ConsoleVariable_t	cvServerGameTime;		// The length of time a round should last.
extern	ConsoleVariable_t	cvServerGameClients;	// Number of clients needed for a round to start.
extern	ConsoleVariable_t	cvServerGravityTweak;	// For detailed "tweaking" without touching the absolute amount. Works like mass...
extern	ConsoleVariable_t	cvServerGravity;		// The absolute gravity amount.
extern	ConsoleVariable_t	cvServerAim;			// Auto-aiming.

#ifdef __cplusplus
extern "C" {
#endif

	void Server_Initialize(void);
	void Server_EntityFrame(ServerEntity_t *eEntity);

	// Server to client
	void Server_KillClient(ServerEntity_t *eClient);
	void Server_UpdateClientMenu(ServerEntity_t *eClient, int iMenuState, bool bShow);

	bool Server_SpawnEntity(ServerEntity_t *ent);

#define Server_PrecacheModel(a) Engine.Server_PrecacheResource(RESOURCE_MODEL,a)
#define	Server_PrecacheSound(a) Engine.Server_PrecacheResource(RESOURCE_SOUND,a)
#define Server_WorldLightStyle(a,b) Engine.LightStyle(a,b)

	/*
		Entity Functions
		*/

	ServerEntity_t	*Entity_Spawn(void);

	bool Entity_CanDamage(ServerEntity_t *eEntity, ServerEntity_t *eTarget, int iDamageType);
	bool Entity_IsPlayer(ServerEntity_t *eEntity);
	bool Entity_IsMonster(ServerEntity_t *eEntity);
	bool Entity_IsTouching(ServerEntity_t *eEntity, ServerEntity_t *eOther);
	bool Entity_DropToFloor(ServerEntity_t *eEntity);

	void Entity_SetOrigin(ServerEntity_t *eEntity, MathVector3_t vOrigin);
	void Entity_SetAngles(ServerEntity_t *eEntity, MathVector3_t vAngles);
	void Entity_SetModel(ServerEntity_t *eEntity, char *cModelPath);
	void Entity_SetSizeVector(ServerEntity_t *eEntity, MathVector3_t vMin, MathVector3_t vMax);
	void Entity_SetSize(ServerEntity_t *eEntity, float fMinA, float fMinB, float fMinC, float fMaxA, float fMaxB, float fMaxC);
	void Entity_SetPhysics(ServerEntity_t *seEntity, PhysicsSolidTypes_t pstSolidType, float fMass, float fFriction);
	void Entity_RadiusDamage(ServerEntity_t *eInflictor, float fRadius, int iDamage, int iDamageType);
	void Entity_Damage(ServerEntity_t *seEntity, ServerEntity_t *seInflictor, int iDamage, DamageType_t dtType);
	void Entity_Remove(ServerEntity_t *eEntity);
	void Entity_CheckFrames(ServerEntity_t *eEntity);
	void Entity_ResetAnimation(ServerEntity_t *eEntity);
	void Entity_Animate(ServerEntity_t *eEntity, EntityFrame_t *efAnimation);
	void Entity_Link(ServerEntity_t *eEntity, bool bTouchTriggers);
	void Entity_Unlink(ServerEntity_t *eEntity);
	void Entity_MakeVectors(ServerEntity_t *eEntity);
	void Entity_AddEffects(ServerEntity_t *eEntity, int iEffects);
	void Entity_RemoveEffects(ServerEntity_t *eEntity, int iEffects);
	void Entity_ClearEffects(ServerEntity_t *eEntity);
	void Entity_AddFlags(ServerEntity_t *eEntity, int iFlags);
	void Entity_RemoveFlags(ServerEntity_t *eEntity, int iFlags);
	void Entity_ClearFlags(ServerEntity_t *eEntity, int iFlags);

#define Entity_SetKilledFunction(a,b) (a->local.KilledFunction = b)
#define Entity_SetDamagedFunction(a,b) (a->local.DamagedFunction = b)
#define	Entity_SetBlockedFunction(a,b) (a->v.BlockedFunction = b)
#define Entity_SetTouchFunction(a,b) (a->v.TouchFunction = b)

	ServerEntity_t	*Entity_SpawnPoint(ServerEntity_t *eEntity, int iType);

#ifdef __cplusplus
};
#endif

#endif
