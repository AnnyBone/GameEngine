/*	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef __SERVERMONSTER_H__
#define __SERVERMONSTER_H__

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
	MONSTER_STATE_DEAD = 1,	// Monster is dead.
	MONSTER_STATE_DYING,	// In-between dead and alive.
	MONSTER_STATE_AWAKE,	// Monster is awake.
	MONSTER_STATE_ASLEEP,	// Monster is asleep.

	MONSTER_STATE_NONE		// Monster has no state.
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
	MONSTER_RELATIONSHIP_HATE,
	RELATIONSHIP_NEUTRAL,
	RELATIONSHIP_LIKE
};

/*	Various thought processes.
*/
typedef enum
{
	MONSTER_THINK_IDLE = 1,		// Monster is idle.
	MONSTER_THINK_FLEEING,		// Monster is fleeing.
	MONSTER_THINK_PURSUING,		// Monster is following.
	MONSTER_THINK_ATTACKING,	// Monster is attacking.
	MONSTER_THINK_WANDERING		// Monster is just walking around.
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

ServerEntity_t *Monster_GetTarget(ServerEntity_t *eMonster);
ServerEntity_t *Monster_GetEnemy(ServerEntity_t *Monster);

bool Monster_SetState(ServerEntity_t *eMonster, MonsterState_t msState);
bool Monster_SetThink(ServerEntity_t *eMonster, MonsterThink_t mtThink);
bool Monster_IsVisible(ServerEntity_t *ent, ServerEntity_t *target);
bool Monster_EmotionReset(ServerEntity_t *eMonster, int iEmotion);

int	Monster_GetRelationship(ServerEntity_t *eMonster, ServerEntity_t *eTarget);

float MONSTER_GetRange(ServerEntity_t *ent, MathVector3f_t target);

void Monster_Damage(ServerEntity_t *target, ServerEntity_t *inflictor, int iDamage, int iDamageType);
void Monster_Jump(ServerEntity_t *eMonster, float fVelocity);
void Monster_MoveToGoal(ServerEntity_t *ent, MathVector3f_t goal, float distance);
void Monster_MoveRandom(ServerEntity_t *eMonster, float fSpeed);
void Monster_Frame(ServerEntity_t *eMonster);
void Monster_Killed(ServerEntity_t *eTarget, ServerEntity_t *eAttacker);

Waypoint_t *Monster_GetMoveTarget(ServerEntity_t *Monster);

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
