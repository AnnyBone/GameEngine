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

#include "server_main.h"

#include "server_ai.h"

void AI_Spawn(ServerEntity_t *entity)
{
	// Reset AI struct for this entity.
	memset(&entity->ai, 0, sizeof(AIVariables_t));

	entity->ai.current_movement = AI_MOVEMENT_RUNNING;
	entity->ai.current_movespeed = 20.0f;
}

///////////////////////////////////////////
// THINK

void AI_SetThink(ServerEntity_t *entity, unsigned int think)
{
	if (entity->ai.current_think == think)
		return;
	else if ((think >= AI_THINK_END) || (think <= AI_THINK_START))
	{
		g_engine->Con_Printf("Invalid think! (%s) (%i)\n", entity->v.cClassname, think);
		return;
	}

	entity->ai.current_think = think;
}

///////////////////////////////////////////
// STATE

void AI_SetState(ServerEntity_t *entity, unsigned int state)
{
	if (entity->ai.current_state == state)
		return;
	else if ((state >= AI_STATE_END) || (state <= AI_STATE_START))
	{
		g_engine->Con_Printf("Invalid state! (%s) (%i)\n", entity->v.cClassname, state);
		return;
	}

	entity->ai.current_state = state;
}

///////////////////////////////////////////
// SIMULATION

/*	Used to go over each monster state then update it, and then calls the monsters
	assigned think function.
*/
void AI_Frame(ServerEntity_t *entity)
{
	// The following is only valid for actual monsters.
	if (!Entity_IsMonster(entity))
		return;

	Entity_CheckFrames(entity);

	// Handle jumping.
	if ((entity->local.jump_velocity < -300.0f) && (entity->v.flags & FL_ONGROUND))
	{
		entity->local.jump_velocity = 0;

		// Call up land function, so custom sounds can be added.
		if (entity->ai.Land)
			entity->ai.Land(entity);
	}
	else if (!(entity->v.flags & FL_ONGROUND))
		entity->local.jump_velocity = entity->v.velocity[2];

	if (entity->ai.Think)
		entity->ai.Think(entity);

	AI_Movement(entity);
}

///////////////////////////////////////////
// MOVEMENT

void AI_Movement(ServerEntity_t *entity)
{
	// Let entities override this.
	if (entity->ai.Movement)
	{
		entity->ai.Movement(entity);
		return;
	}

	// We don't move while asleep.
	if ((entity->ai.current_state == AI_STATE_ASLEEP) || (entity->ai.current_state == AI_STATE_DEAD))
		return;

	switch (entity->ai.current_movement)
	{
	default:
	case AI_MOVEMENT_RUNNING:
		AI_RunMovement(entity);
		break;
	case AI_MOVEMENT_FLYING:
		AI_FlyMovement(entity);
		break;
	}
}