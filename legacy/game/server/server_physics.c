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

#include "server_physics.h"

/*	Server-Side Physics	*/

#define PHYSICS_MAXVELOCITY	2000.0f

/*	Sets the amount of gravity for the object.
*/
void Physics_SetGravity(ServerEntity_t *eEntity)
{	
	eEntity->v.velocity[2] -=
		// Slightly more complex gravity management.
		(eEntity->Physics.gravity * cvServerGravityTweak.value) *
		// Multiplied by the mass of the entity.
		eEntity->Physics.mass *
		(float)Engine.Server_GetFrameTime();
}

bool Physics_CheckWater(ServerEntity_t *eEntity)
{
	plVector3f_t point;
	int	cont;

	point[0] = eEntity->v.origin[0];
	point[1] = eEntity->v.origin[1];
	point[2] = eEntity->v.origin[2] + eEntity->v.mins[2] + 1.0f;

	eEntity->v.waterlevel = 0;
	eEntity->v.watertype = BSP_CONTENTS_EMPTY;
	
	cont = Engine.Server_PointContents(point);
	if (cont <= BSP_CONTENTS_WATER)
	{
		eEntity->v.watertype = cont;
		eEntity->v.waterlevel = 1.0f;

		point[2] = eEntity->v.origin[2] + (eEntity->v.mins[2] + eEntity->v.maxs[2])*0.5f;

		if (Engine.Server_PointContents(point) <= BSP_CONTENTS_WATER)
		{
			eEntity->v.waterlevel = 2.0f;

			point[2] = eEntity->v.origin[2] + eEntity->v.view_ofs[2];

			if (Engine.Server_PointContents(point) <= BSP_CONTENTS_WATER)
				eEntity->v.waterlevel = 3.0f;
		}
	}

	return eEntity->v.waterlevel > 1;
}

void Physics_CheckVelocity(ServerEntity_t *eEntity)
{
	int i;

	for(i = 0; i < 3; i++)
	{
		if (PL_ISNAN(eEntity->v.velocity[i]))
		{
			Engine.Con_DPrintf("Got a NaN velocity on %s\n",eEntity->v.cClassname);
			eEntity->v.velocity[i] = 0;
		}

		if (PL_ISNAN(eEntity->v.origin[i]))
		{
			Engine.Con_DPrintf("Got a NaN origin on %s\n",eEntity->v.cClassname);
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

void Physics_WallFriction(ServerEntity_t *eEntity, trace_t *trLine)
{
	MathVector3f_t forward, right, up;
	float d, i;
	MathVector3f_t into, side;

	plAngleVectors(eEntity->v.v_angle, forward, right, up);
	d = Math_DotProduct(trLine->plane.normal, forward);

	d += 0.5;
	if (d >= 0)
		return;

	// Cut the tangential velocity.
	i = Math_DotProduct(trLine->plane.normal, eEntity->v.velocity);
	plVectorScalef(trLine->plane.normal, i, into);
	plVectorSubtract3fv(eEntity->v.velocity, into, side);

	eEntity->v.velocity[0] = side[0] * (1 + d);
	eEntity->v.velocity[1] = side[1] * (1 + d);
}

/*	Two entities have touched, so run their touch functions
*/
void Physics_Impact(ServerEntity_t *eEntity,ServerEntity_t *eOther)
{
	// Entities using noclip shouldn't "impact" anything.
	if (eOther->v.movetype == MOVETYPE_NOCLIP)
		return;

	if(eEntity->v.TouchFunction && eEntity->Physics.solid != SOLID_NOT)
		eEntity->v.TouchFunction(eEntity,eOther);

	if(eOther->v.TouchFunction && eOther->Physics.solid != SOLID_NOT)
		eOther->v.TouchFunction(eOther,eEntity);
}

/*
	Push Move
*/

/*	Does not change the entities velocity at all
*/
trace_t Physics_PushEntity(ServerEntity_t *eEntity, MathVector3f_t mvPush)
{
	trace_t	trace;
	MathVector3f_t	end;

	plVectorAdd3fv(eEntity->v.origin, mvPush, end);
	
	if (eEntity->v.movetype == (MOVETYPE_FLYMISSILE || MOVETYPE_FLYBOUNCE))
		trace = Engine.Server_Move(eEntity->v.origin, eEntity->v.mins, eEntity->v.maxs, end, MOVE_MISSILE, eEntity);
	else if (eEntity->Physics.solid == SOLID_TRIGGER || eEntity->Physics.solid == SOLID_NOT)
		// only clip against bmodels
		trace = Engine.Server_Move(eEntity->v.origin, eEntity->v.mins, eEntity->v.maxs, end, MOVE_NOMONSTERS, eEntity);
	else
		trace = Engine.Server_Move(eEntity->v.origin, eEntity->v.mins, eEntity->v.maxs, end, MOVE_NORMAL, eEntity);

	plVectorCopy(trace.endpos, eEntity->v.origin);
	Entity_Link(eEntity, true);

	if (trace.ent)
		Physics_Impact(eEntity, trace.ent);

	return trace;
}