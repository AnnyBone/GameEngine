/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __SERVERMAIN__
#define __SERVERMAIN__

#include "game_main.h"

#include "shared_game.h"
#include "shared_server.h"

// Base includes
#include "server_physics.h"
#include "server_mode.h"
#include "server_monster.h"
#include "server_misc.h"

extern	cvar_t	cvServerSkill;			// The difficulty level.
extern	cvar_t	cvServerSelfDamage;		// If the player can directly kill themselves or not.
extern	cvar_t	cvServerMaxHealth;		// The maximum amount of health that a player can have.
extern	cvar_t	cvServerMonsters;		// Should monsters be allowed?
extern	cvar_t	cvServerMaxScore;		// Maximum score before a round ends.
extern	cvar_t	cvServerGameMode;		// The gamemode being used for the server.
extern	cvar_t	cvServerPlayerModel;	// The default player model for clients joining the server.
extern	cvar_t	cvServerRespawnDelay;	// The delay before respawning.
extern	cvar_t	cvServerWaypointSpawn;	// Should waypoints spawn?
extern	cvar_t	cvServerBots;			// Shoulds bots spawn?
extern	cvar_t	cvServerDefaultHealth;	// Default amount of health for a player.
extern	cvar_t	cvServerWaypointDelay;	// Delay between each check before spawning another waypoint.
extern	cvar_t	cvServerWaypointParse;	// File to parse for waypoint positions.
extern	cvar_t	cvServerGameTime;		// The length of time a round should last.
extern	cvar_t	cvServerGameClients;	// Number of clients needed for a round to start.
extern	cvar_t	cvServerGravityTweak;	// For detailed "tweaking" without touching the absolute amount. Works like mass...
extern	cvar_t	cvServerGravity;		// The absolute gravity amount.
extern	cvar_t	cvServerAim;			// Auto-aiming.

void Server_Initialize(void);
void Server_EntityFrame(edict_t *eEntity);

// Server to client
void Server_KillClient(edict_t *eClient);
void Server_UpdateClientMenu(edict_t *eClient,int iMenuState,bool bShow);

bool Server_SpawnEntity(edict_t *ent);

/*
	Entity Functions
*/

edict_t	*Entity_Spawn(void);

bool Entity_CanDamage(edict_t *eEntity, edict_t *eTarget, int iDamageType);
bool Entity_IsPlayer(edict_t *eEntity);
bool Entity_IsMonster(edict_t *eEntity);
bool Entity_IsTouching(edict_t *eEntity, edict_t *eOther);
bool Entity_DropToFloor(edict_t *eEntity);

void Entity_SetOrigin(edict_t *eEntity, MathVector3_t vOrigin);
void Entity_SetAngles(edict_t *eEntity, MathVector3_t vAngles);
void Entity_SetModel(edict_t *eEntity, char *cModelPath);
void Entity_SetSizeVector(edict_t *eEntity, MathVector3_t vMin, MathVector3_t vMax);
void Entity_SetSize(edict_t *eEntity, float fMinA, float fMinB, float fMinC, float fMaxA, float fMaxB, float fMaxC);
void Entity_RadiusDamage(edict_t *eInflictor, float fRadius, int iDamage, int iDamageType);
void Entity_Remove(edict_t *eEntity);
void Entity_CheckFrames(edict_t *eEntity);
void Entity_ResetAnimation(edict_t *eEntity);
void Entity_Animate(edict_t *eEntity, EntityFrame_t *efAnimation);
void Entity_Link(edict_t *eEntity, bool bTouchTriggers);
void Entity_Unlink(edict_t *eEntity);
void Entity_MakeVectors(edict_t *eEntity);
void Entity_AddEffects(edict_t *eEntity, int iEffects);
void Entity_RemoveEffects(edict_t *eEntity, int iEffects);
void Entity_ClearEffects(edict_t *eEntity);
void Entity_AddFlags(edict_t *eEntity, int iFlags);
void Entity_RemoveFlags(edict_t *eEntity, int iFlags);
void Entity_ClearFlags(edict_t *eEntity, int iFlags);

#define	ENTITY_REMOVE(a)	{	Entity_Remove(a);	return;	}

edict_t	*Entity_SpawnPoint(edict_t *eEntity,int iType);

#endif
