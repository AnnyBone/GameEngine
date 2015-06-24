/*	Copyright (C) 2011-2014 OldTimes Software
*/
#ifndef __SERVERMONSTER__
#define __SERVERMONSTER__

#include "server_main.h"

//#define	MONSTER_DEBUG

typedef struct
{
	int	iFirstType,iSecondType,	// The first and second monster types, which determines the encounter we might be dealing with.
		iRelationship;			// The relationship type between the two.
} MonsterRelationship_t;

#define MONSTER_NONE		0	// Used for entities that should not be treated as monsters.
#define MONSTER_PLAYER		1	// Clients / bots
#define	MONSTER_VEHICLE		2	// Vehicles
// Anything greater than 2 is assumed to be a monster by the engine!!!
#ifdef GAME_OPENKATANA
#define	MONSTER_PRISONER	3
#define	MONSTER_LASERGAT	4
#define	MONSTER_INMATER		5

#define MONSTER_MIKIKO      50  // Added for sanity sake.
#define MONSTER_SUPERFLY    51  // Added for sanity sake.
#elif GAME_ADAMAS
#define MONSTER_HURLER	    3
#endif

/*	Various states for the AI.
*/
typedef enum
{
	STATE_DEAD		= 1,	// Monster is dead
	STATE_AWAKE,			// Monster is awake
	STATE_ASLEEP,			// Monster is asleep

	STATE_NONE				// Monster has no state
} MonsterState_t;

#define	MONSTER_RANGE_MELEE		48.0f
#define MONSTER_RANGE_NEAR		256.0f
#define MONSTER_RANGE_MEDIUM	700.0f
#define MONSTER_RANGE_FAR		2000.0f

/*	Relationship states.
	Neutral is the default relationship state.
*/
enum
{
	RELATIONSHIP_HATE,
	RELATIONSHIP_NEUTRAL,
	RELATIONSHIP_LIKE
};

/*	Various thought processes.
*/
typedef enum
{
	THINK_IDLE = 1,		// Monster is idle.
	THINK_FLEEING,		// Monster is fleeing.
	THINK_PURSUING,		// Monster is following.
	THINK_ATTACKING,	// Monster is attacking.
	THINK_WANDERING		// Monster is just walking around.
} MonsterThink_t;

/*	Various emotions.
*/
enum
{
	EMOTION_BOREDOM,
	EMOTION_FEAR,
	EMOTION_JOY,
	EMOTION_ANGER,
	EMOTION_SADNESS,
	EMOTION_DISGUST,
	EMOTION_SURPRISE,
	EMOTION_CONTEMPT,
	EMOTION_INTEREST,

	EMOTION_NONE
};

ServerEntity_t	*Monster_GetTarget(ServerEntity_t *eMonster);

bool Monster_SetState(ServerEntity_t *eMonster, MonsterState_t msState);
bool Monster_SetThink(ServerEntity_t *eMonster, MonsterThink_t mtThink);
bool Monster_IsVisible(ServerEntity_t *ent, ServerEntity_t *target);
bool Monster_EmotionReset(ServerEntity_t *eMonster, int iEmotion);

int	Monster_GetRelationship(ServerEntity_t *eMonster, ServerEntity_t *eTarget);

float MONSTER_GetRange(ServerEntity_t *ent, vec3_t target);

void Monster_Spawn(ServerEntity_t *eMonster);
void Monster_Damage(ServerEntity_t *target, ServerEntity_t *inflictor, int iDamage, int iDamageType);
void Monster_Jump(ServerEntity_t *eMonster, float fVelocity);
void Monster_MoveToGoal(ServerEntity_t *ent, vec3_t goal, float distance);
void Monster_MoveRandom(ServerEntity_t *eMonster, float fSpeed);
void Monster_Frame(ServerEntity_t *eMonster);
void Monster_SetTargets(ServerEntity_t *eMonster);
void Monster_Killed(ServerEntity_t *eTarget, ServerEntity_t *eAttacker);

/*
    Bots
*/

enum
{
	BOT_DEFAULT,
#ifdef OPENKATANA
	BOT_MIKIKO,
	BOT_SUPERFLY
#endif
};

#endif
