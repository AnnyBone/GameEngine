/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
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

void AI_Spawn(ServerEntity_t *entity) {
    // Reset AI struct for this entity.
    memset(&entity->ai, 0, sizeof(AIVariables_t));

    entity->ai.current_movement = AI_MOVEMENT_RUNNING;
    entity->ai.current_movespeed = 20.0f;
}

///////////////////////////////////////////
// THINK

void AI_SetThink(ServerEntity_t *entity, unsigned int think) {
    if (entity->ai.current_think == think)
        return;
    else if (think <= AI_THINK_START) {
        g_engine->Con_Printf("Invalid think! (%s) (%i)\n", entity->v.cClassname, think);
        return;
    }

    entity->ai.current_think = think;
}

///////////////////////////////////////////
// STATE

void AI_SetState(ServerEntity_t *entity, unsigned int state) {
    if (entity->ai.current_state == state)
        return;
    else if (state <= AI_STATE_START) {
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
void AI_Frame(ServerEntity_t *entity) {
    // The following is only valid for actual monsters.
    if (!Entity_IsMonster(entity))
        return;

    Entity_CheckFrames(entity);

    // Handle jumping.
    if ((entity->local.jump_velocity < -300.0f) && (entity->v.flags & FL_ONGROUND)) {
        entity->local.jump_velocity = 0;

        // Call up land function, so custom sounds can be added.
        if (entity->ai.Land)
            entity->ai.Land(entity);
    } else if (!(entity->v.flags & FL_ONGROUND))
        entity->local.jump_velocity = entity->v.velocity[2];

    if (entity->ai.Think)
        entity->ai.Think(entity);

    AI_Movement(entity);
}

///////////////////////////////////////////
// MOVEMENT

void AI_Movement(ServerEntity_t *entity) {
    // Let entities override this.
    if (entity->ai.Movement) {
        entity->ai.Movement(entity);
        return;
    }

    // We don't move while asleep.
    if ((entity->ai.current_state == AI_STATE_ASLEEP) || (entity->ai.current_state == AI_STATE_DEAD))
        return;

    switch (entity->ai.current_movement) {
        default:
        case AI_MOVEMENT_RUNNING:
            AI_RunMovement(entity);
            break;
        case AI_MOVEMENT_FLYING:
            AI_FlyMovement(entity);
            break;
    }
}

// Flying
void AI_FlyMovement(ServerEntity_t *entity) {
    // Ensure we're on the ground?
    if (Entity_IsOnGround(entity))
        return;

    if (entity->ai.target_move) {
        // todo: interp for this over a few frames...

        plVector3f_t angles;
        plVectorSubtract3fv(entity->v.origin, entity->ai.target_move->position, angles);
        plVectorNormalize(angles);

        MathVector_t vangles = plVectorToAngles(angles);
        entity->v.angles[PL_YAW] = -vangles.vY;
        entity->v.v_angle[PL_YAW] = entity->v.angles[PL_YAW];

        AI_ForwardMovement(entity, entity->ai.current_movespeed);

        // Link us up, Scotty(?)
        Entity_Link(entity, false);

        if (plVectorDifference(entity->v.origin, entity->ai.target_move->position) <= MONSTER_RANGE_MEDIUM) {
            g_engine->Con_DPrintf("Arrived at target!\n");

            entity->ai.target_move = nullptr;
            return;
        }
    }
}

// Running
void AI_RunMovement(ServerEntity_t *entity) {
    // Ensure we're on the ground?
    if (!Entity_IsOnGround(entity))
        return;
}

void AI_ForwardMovement(ServerEntity_t *entity, float velocity) {
    plVector3f_t end;

    Entity_MakeVectors(entity);
    Math_VectorMA(entity->v.origin, velocity, entity->local.vForward, end);
    plVectorScalef(entity->local.vForward, velocity, entity->v.velocity);
}

/*	Allows a monster to jump with the given velocity.
*/
void AI_JumpMovement(ServerEntity_t *entity, float velocity) {
    if (entity->v.velocity[2] != 0 || Entity_IsOnGround(entity))
        return;

    // Allow the monster to add additional sounds/movement if required.
    if (entity->ai.Jump)
        entity->ai.Jump(entity);

    entity->v.flags -= FL_ONGROUND;
    entity->v.velocity[2] = velocity;
}

/*	Can be used to debug monster movement / apply random movement.
*/
void AI_RandomMovement(ServerEntity_t *entity, float velocity) {
    if (rand() % 50 == 0) {
        int result = rand() % 3;
        if (result == 0)
            entity->v.velocity[0] += velocity;
        else if (result == 1)
            entity->v.velocity[0] -= velocity;

        result = rand() % 3;
        if (result == 0)
            entity->v.velocity[1] += velocity;
        else if (result == 1)
            entity->v.velocity[1] -= velocity;

        entity->v.angles[1] = plVectorToYaw(entity->v.velocity);
    } else if (rand() % 150 == 0)
        AI_JumpMovement(entity, 200.0f);
    else if (rand() % 250 == 0)
        entity->v.angles[1] = (float) (rand() % 360);
}