/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
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
    if (entity->ai.current_think == think) {
        return;
    } else if (think <= AI_THINK_START) {
        g_engine->Con_Printf("Invalid think! (%s) (%i)\n", entity->v.classname, think);
        return;
    }

    entity->ai.current_think = think;
}

///////////////////////////////////////////
// STATE

void AI_SetState(ServerEntity_t *entity, unsigned int state)
{
    if (entity->ai.current_state == state) {
        return;
    } else if (state <= AI_STATE_START) {
        g_engine->Con_Printf("Invalid state! (%s) (%i)\n", entity->v.classname, state);
        return;
    }

    entity->ai.current_state = state;
}

///////////////////////////////////////////
// SIMULATION

/*
Used to go over each monster state then update it, and then calls the monsters
assigned think function.
*/
void AI_Frame(ServerEntity_t *entity) {
    // The following is only valid for actual monsters.
    if (!Entity_IsMonster(entity)) {
        return;
    }

    Entity_CheckFrames(entity);

    // Handle jumping.
    if ((entity->local.jump_velocity < -300.0f) && (entity->v.flags & FL_ONGROUND)) {
        entity->local.jump_velocity = 0;

        // Call up land function, so custom sounds can be added.
        if (entity->ai.Land)
            entity->ai.Land(entity);
    } else if (!(entity->v.flags & FL_ONGROUND)) {
        entity->local.jump_velocity = entity->v.velocity.y;
    }

    if (entity->ai.Think) {
        entity->ai.Think(entity);
    }

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
    if ((entity->ai.current_state == AI_STATE_ASLEEP) || (entity->ai.current_state == AI_STATE_DEAD)) {
        return;
    }

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

// Flying
void AI_FlyMovement(ServerEntity_t *entity) {
    // Ensure we're on the ground?
    if (Entity_IsOnGround(entity)) {
        return;
    }

    if (entity->ai.target_move) {
        // todo: interp for this over a few frames...

        PLVector3D angles = entity->v.origin - entity->ai.target_move->position;
        angles.Normalize();

        entity->v.angles.y = -angles.y;
        entity->v.v_angle.y = entity->v.angles.y;

        AI_ForwardMovement(entity, entity->ai.current_movespeed);

        // Link us up, Scotty(?)
        Entity_Link(entity, false);

        if (entity->v.origin.Difference(entity->ai.target_move->position) <= MONSTER_RANGE_MEDIUM) {
            g_engine->Con_DPrintf("Arrived at target!\n");

            entity->ai.target_move = nullptr;
            return;
        }
    }
}

// Running
void AI_RunMovement(ServerEntity_t *entity) {
    // Ensure we're on the ground?
    if (!Entity_IsOnGround(entity)) {
        return;
    }
}

void AI_ForwardMovement(ServerEntity_t *entity, float velocity) {
#if 0 // todo: I'm not sure what this was intended to do here... So leaving it for now!
    PLVector3D end;

	Entity_MakeVectors(entity);

    Math_MAVector3D()
	Math_VectorMA(entity->v.origin, velocity, entity->local.forward, end);

    entity->v.velocity = entity->local.forward * velocity;
#endif
}

// Allows a monster to jump with the given velocity.
void AI_JumpMovement(ServerEntity_t *entity, float velocity) {
    if (entity->v.velocity.z != 0 || Entity_IsOnGround(entity)) {
        return;
    }

    // Allow the monster to add additional sounds/movement if required.
    if (entity->ai.Jump) {
        entity->ai.Jump(entity);
    }

    entity->v.flags -= FL_ONGROUND;
    entity->v.velocity.z = velocity;
}

// Can be used to debug monster movement / apply random movement.
void AI_RandomMovement(ServerEntity_t *entity, float velocity) {
    if (rand() % 50 == 0) {
        int result = rand() % 3;
        if (result == 0) {
            entity->v.velocity.x += velocity;
        } else if (result == 1) {
            entity->v.velocity.x -= velocity;
        }

        result = rand() % 3;
        if (result == 0) {
            entity->v.velocity.y += velocity;
        } else if (result == 1) {
            entity->v.velocity.y -= velocity;
        }

        entity->v.angles.y = Math_VectorToYaw(entity->v.velocity);
    }
    else if (rand() % 150 == 0) {
        AI_JumpMovement(entity, 200.0f);
    } else if (rand() % 250 == 0) {
        entity->v.angles.y = (float) (rand() % 360);
    }
}