/*
Copyright (C) 2011-2016 OldTimes Software

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

#include <shared_game.h>
#include "server_physics.h"

/*	Server-Side Physics	*/

#define PHYSICS_MAXVELOCITY	2000.0f

// Sets the amount of gravity for the object.
void Physics_SetGravity(ServerEntity_t *entity) {
    if(entity->Physics.mass == 0) {
        return;
    }

    // force = (cv_server_gravity.value * entity->Physics.mass) / distance_ground ?

    // todo, rewrite this sensibly... urgh
	entity->v.velocity[2] -=
		(entity->Physics.gravity * cvServerGravityTweak.value) *
		// Multiplied by the mass of the entity.
		entity->Physics.mass *
		(float)Engine.Server_GetFrameTime();
}

bool Physics_CheckWater(ServerEntity_t *entity)
{
	PLVector3D point;
	point[0] = entity->v.origin[0];
	point[1] = entity->v.origin[1];
	point[2] = entity->v.origin[2] + entity->v.mins[2] + 1.0f;

	entity->v.waterlevel = 0;
	entity->v.watertype = BSP_CONTENTS_EMPTY;
	
	int cont = Engine.Server_PointContents(point);
	if (cont <= BSP_CONTENTS_WATER) {
		entity->v.watertype = cont;
		entity->v.waterlevel = 1.0f;

		point[2] = entity->v.origin[2] + (entity->v.mins[2] + entity->v.maxs[2])*0.5f;

		if (Engine.Server_PointContents(point) <= BSP_CONTENTS_WATER) {
			entity->v.waterlevel = 2.0f;

			point[2] = entity->v.origin[2] + entity->v.view_ofs[2];

			if (Engine.Server_PointContents(point) <= BSP_CONTENTS_WATER)
				entity->v.waterlevel = 3.0f;
		}
	}

	return entity->v.waterlevel > 1;
}

void Physics_CheckVelocity(ServerEntity_t *eEntity)
{
	int i;

	for(i = 0; i < 3; i++)
	{
		if (MATH_ISNAN(eEntity->v.velocity[i]))
		{
			Engine.Con_DPrintf("Got a NaN velocity on %s\n",eEntity->v.classname);
			eEntity->v.velocity[i] = 0;
		}

		if (MATH_ISNAN(eEntity->v.origin[i]))
		{
			Engine.Con_DPrintf("Got a NaN origin on %s\n",eEntity->v.classname);
			eEntity->v.origin[i] = 0;
		}

		if(eEntity->v.velocity[i] > PHYSICS_MAXVELOCITY)
			eEntity->v.velocity[i] = PHYSICS_MAXVELOCITY;
		else if(eEntity->v.velocity[i] < -PHYSICS_MAXVELOCITY)
			eEntity->v.velocity[i] = -PHYSICS_MAXVELOCITY;
	}
}

void Physics_CheckWaterTransition(ServerEntity_t *eEntity)
{
	int	iCont = Engine.Server_PointContents(eEntity->v.origin);

	if(!eEntity->v.watertype)
	{
		eEntity->v.watertype	= iCont;
		eEntity->v.waterlevel	= 1.0f;
		return;
	}

	if(iCont <= BSP_CONTENTS_WATER)
	{
		eEntity->v.watertype	= iCont;
		eEntity->v.waterlevel	= 1.0f;

		Sound(eEntity,CHAN_AUTO,PHYSICS_SOUND_SPLASH,15,ATTN_NORM);
		return;
	}
	else
	{
		eEntity->v.watertype	= BSP_CONTENTS_EMPTY;
		eEntity->v.waterlevel	= (float)iCont;
	}
	
	//Sound(eEntity, CHAN_AUTO, PHYSICS_SOUND_BODY, 15, ATTN_NORM);
}

void Physics_WallFriction(ServerEntity_t *entity, trace_t *trace) {
    PLVector3D forward, right, up;
	Math_AngleVectors(entity->v.v_angle, &forward, &right, &up);
    float d = trace->plane.normal.DotProduct(forward);

	d += 0.5;
	if (d >= 0) {
		return;
	}

	// Cut the tangential velocity.
	float i = entity->v.velocity.DotProduct(trace->plane.normal);
    PLVector3D into = trace->plane.normal * i;
    PLVector3D side = entity->v.velocity - into;

	entity->v.velocity.x = side.x * (1 + d);
	entity->v.velocity.y = side.y * (1 + d);
}

/*	Two entities have touched, so run their touch functions
*/
void Physics_Impact(ServerEntity_t *entity, ServerEntity_t *other) {
	// Entities using noclip shouldn't "impact" anything.
	if (other->v.movetype == MOVETYPE_NOCLIP) {
        return;
    }

	if(entity->v.TouchFunction && entity->Physics.solid != SOLID_NOT) {
        entity->v.TouchFunction(entity, other);
    }

	if(other->v.TouchFunction && other->Physics.solid != SOLID_NOT) {
        other->v.TouchFunction(other, entity);
    }
}

/*
	Push Move
*/

// Does not change the entities velocity at all.
trace_t Physics_PushEntity(ServerEntity_t *entity, PLVector3D push) {
	PLVector3D end = entity->v.origin + push;

    trace_t	trace;
	if ((entity->v.movetype == MOVETYPE_FLYMISSILE) || (entity->v.movetype == MOVETYPE_FLYBOUNCE)) {
        trace = Engine.Server_Move(entity->v.origin, entity->v.mins, entity->v.maxs, end, MOVE_MISSILE, entity);
    } else if (entity->Physics.solid == SOLID_TRIGGER || entity->Physics.solid == SOLID_NOT) {
        // only clip against bmodels
        trace = Engine.Server_Move(entity->v.origin, entity->v.mins, entity->v.maxs, end, MOVE_NOMONSTERS, entity);
    } else {
        trace = Engine.Server_Move(entity->v.origin, entity->v.mins, entity->v.maxs, end, MOVE_NORMAL, entity);
    }

	entity->v.origin = trace.endpos;
	Entity_Link(entity, true);

	if (trace.ent) {
        Physics_Impact(entity, trace.ent);
    }

	return trace;
}