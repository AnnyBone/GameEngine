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

plEXTERN_C_START

void AI_Spawn(ServerEntity_t *entity);
void AI_Frame(ServerEntity_t *entity);

void AI_SetThink(ServerEntity_t *entity, unsigned int think);
void AI_SetState(ServerEntity_t *entity, unsigned int state);

// AI Movement
void AI_Movement(ServerEntity_t *entity);
void AI_RunMovement(ServerEntity_t *entity);
void AI_FlyMovement(ServerEntity_t *entity);
void AI_ForwardMovement(ServerEntity_t *entity, float velocity);

plEXTERN_C_END