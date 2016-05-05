/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

/*	Various states for the AI.
*/
typedef enum AIState_e
{
	AI_STATE_START,

	AI_STATE_DEAD,		// Monster is dead.
	AI_STATE_DYING,		// In-between dead and alive.
	AI_STATE_AWAKE,		// Monster is awake.
	AI_STATE_ASLEEP,	// Monster is asleep.

	AI_STATE_END
	// Anything added after this will be
	// treated as a custom state.
} AIState_t;

/*	Various thought processes.
*/
typedef enum AIThink_s
{
	AI_THINK_START,

	AI_THINK_IDLE,			// Monster is idle.
	AI_THINK_FLEEING,		// Monster is fleeing.
	AI_THINK_PURSUING,		// Monster is following.
	AI_THINK_ATTACKING,		// Monster is attacking.
	AI_THINK_WANDERING,		// Monster is just walking around.

	AI_THINK_END
	// Anything added after this will be
	// treated as a custom think.
} AIThink_t;

/*	Various emotions.
*/
typedef enum AIEmotion_s
{
	AI_EMOTION_START,

	AI_EMOTION_BOREDOM,
	AI_EMOTION_FEAR,
	AI_EMOTION_JOY,
	AI_EMOTION_ANGER,
	AI_EMOTION_SADNESS,
	AI_EMOTION_DISGUST,
	AI_EMOTION_SURPRISE,
	AI_EMOTION_CONTEMPT,
	AI_EMOTION_INTEREST,

	AI_EMOTION_END
	// Anything added after this will be
	// treated as a custom emotion.
} AIEmotion_t;

enum
{
	AI_MOVEMENT_START,

	AI_MOVEMENT_RUNNING,	// Walking / Running
	AI_MOVEMENT_FLYING,
	AI_MOVEMENT_SWIMMING,

	AI_MOVEMENT_END
	// Anything added after this will be
	// treated as a custom movement.
};

/*	Relationship states.
	Neutral is the default relationship state.
*/
enum
{
	AI_RELATIONSHIP_START,

	AI_RELATIONSHIP_HATE,
	AI_RELATIONSHIP_NEUTRAL,
	AI_RELATIONSHIP_LIKE,

	AI_RELATIONSHIP_END
};

plEXTERN_C_START

void AI_Spawn(ServerEntity_t *entity);
void AI_Frame(ServerEntity_t *entity);

void AI_SetThink(ServerEntity_t *entity, unsigned int think);
void AI_SetState(ServerEntity_t *entity, unsigned int state);

// AI Movement
void AI_Movement(ServerEntity_t *entity);
void AI_RunMovement(ServerEntity_t *entity);
void AI_FlyMovement(ServerEntity_t *entity);
void AI_JumpMovement(ServerEntity_t *entity, float velocity);
void AI_ForwardMovement(ServerEntity_t *entity, float velocity);
void AI_RandomMovement(ServerEntity_t *entity, float velocity);

plEXTERN_C_END