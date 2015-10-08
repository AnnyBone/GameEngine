/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

#include "EngineBase.h"

#include "EngineServerPhysics.h"
#include "EngineGame.h"

/*
	pushmove objects do not obey gravity, and do not interact with each other or trigger fields, but block normal movement and push normal objects when they move.

	onground is set for toss objects when they come to a complete rest.  it is set for steping or walking objects

	doors, plats, etc are SOLID_BSP, and MOVETYPE_PUSH
	bonus items are SOLID_TRIGGER touch, and MOVETYPE_TOSS
	corpses are SOLID_NOT and MOVETYPE_TOSS
	crates are SOLID_BBOX and MOVETYPE_TOSS
	walking monsters are SOLID_SLIDEBOX and MOVETYPE_STEP
	flying/floating monsters are SOLID_SLIDEBOX and MOVETYPE_FLY

	solid_edge items only clip against bsp models.
*/

ConsoleVariable_t
	cvPhysicsStopSpeed	= {	"physics_stopspeed",	"100"		},
	cvPhysicsNoStep		= {	"physics_nostep",		"0"			},
	cvPhysicsStepSize	= {	"physics_stepsize",		"18"		};

void Physics_Toss(ServerEntity_t *ent);

void SV_CheckAllEnts (void)
{
	int	e;
	ServerEntity_t *check;

	// See if any solid entities are inside the final position
	check = NEXT_EDICT(sv.edicts);
	for(e = 1; e < sv.num_edicts; e++, check = NEXT_EDICT(check))
	{
		if (check->free)
			continue;
		if(	check->v.movetype == MOVETYPE_PUSH	||
			check->v.movetype == MOVETYPE_NONE	||
			check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		if (SV_TestEntityPosition (check))
			Con_Warning("Entity in invalid position!\n");
	}
}

/*	Runs thinking code if time.  There is some play in the exact time the think
	function will be called, because it is called before any movement is done
	in a frame.  Not used for pushmove objects, because they must be exact.
	Returns false if the entity removed itself.
*/
bool Server_RunThink(ServerEntity_t *ent)
{
	double dThinkTime;

	dThinkTime = ent->v.dNextThink;
	if(dThinkTime <= 0 || dThinkTime > sv.time+host_frametime)
		return true;

	if(dThinkTime < sv.time)
		dThinkTime = sv.time;	// don't let things stay in the past.
								// it is possible to start that way
								// by a trigger with a local time.
	ent->v.dNextThink = 0;

	pr_global_struct.eOther	= sv.edicts;

	if(ent->v.think)
		ent->v.think(ent);

	return !ent->free;
}

/*	Slide off of the impacting object
	returns the blocked flags (1 = floor, 2 = step / wall)
*/
#define	STOP_EPSILON	0.1

int ClipVelocity (MathVector3f_t in, MathVector3f_t normal, MathVector3f_t out, float overbounce)
{
	float	backoff;
	float	change;
	int		i, blocked;

	blocked = 0;
	if (normal[2] > 0)
		blocked |= 1;		// floor
	if (!normal[2])
		blocked |= 2;		// step

	backoff = Math_DotProduct (in, normal) * overbounce;

	for (i=0 ; i<3 ; i++)
	{
		change = normal[i]*backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}


/*	The basic solid body movement clip that slides along multiple planes
	Returns the clipflags if the velocity was modified (hit something solid)
	1 = floor
	2 = wall / step
	4 = dead stop
	If steptrace is not NULL, the trace of any vertical wall hit will be stored
*/
#define	MAX_CLIP_PLANES	5
int SV_FlyMove (ServerEntity_t *ent, float time, trace_t *steptrace)
{
	int					bumpcount, numbumps;
	MathVector3f_t		dir;
	float				d;
	int					numplanes;
	MathVector3f_t		planes[MAX_CLIP_PLANES];
	MathVector3f_t		primal_velocity, original_velocity, new_velocity;
	int					i, j;
	trace_t				trace;
	MathVector3f_t		end;
	float				time_left;
	int					blocked;

	numbumps = 4;

	blocked = 0;
	Math_VectorCopy (ent->v.velocity, original_velocity);
	Math_VectorCopy (ent->v.velocity, primal_velocity);
	numplanes = 0;

	time_left = time;

	for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
	{
		if (!ent->v.velocity[0] && !ent->v.velocity[1] && !ent->v.velocity[2])
			break;

		for (i=0 ; i<3 ; i++)
			end[i] = ent->v.origin[i] + time_left * ent->v.velocity[i];

		trace = SV_Move (ent->v.origin, ent->v.mins, ent->v.maxs, end, FALSE, ent);
		if(trace.bAllSolid)
		{
			// Entity is trapped in another solid
			Math_VectorCopy(g_mvOrigin3f, ent->v.velocity);
			return 3;
		}

		if (trace.fraction > 0)
		{
			// Actually covered some distance
			Math_VectorCopy (trace.endpos, ent->v.origin);
			Math_VectorCopy (ent->v.velocity, original_velocity);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			 break;		// moved the entire distance

		if(!trace.ent)
		{
			Sys_Error ("SV_FlyMove: !trace.ent");
			return 0;
		}

		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
			if (trace.ent->Physics.iSolid == SOLID_BSP)
			{
				ent->v.flags		|= FL_ONGROUND;
				ent->v.groundentity = trace.ent;
			}
		}

		if (!trace.plane.normal[2])
		{
			blocked |= 2;		// step
			if (steptrace)
				*steptrace = trace;	// save for player extrafriction
		}

		// Run the impact function
		Game->Physics_Impact(ent,trace.ent);
		if (ent->free)
			break;		// removed by the impact function

		time_left -= time_left * trace.fraction;

		// Cliped to another plane
		if(numplanes >= MAX_CLIP_PLANES)
		{
			// This shouldn't really happen
			Math_VectorCopy(g_mvOrigin3f, ent->v.velocity);
			return 3;
		}

		Math_VectorCopy (trace.plane.normal, planes[numplanes]);
		numplanes++;

		// Modify original_velocity so it parallels all of the clip planes
		for(i = 0; i < numplanes; i++)
		{
			ClipVelocity (original_velocity, planes[i], new_velocity, 1);
			for (j=0 ; j<numplanes ; j++)
				if (j != i)
				{
					if(Math_DotProduct(new_velocity,planes[j]) < 0)
						break;	// not ok
				}
			if (j == numplanes)
				break;
		}

		if (i != numplanes)
			Math_VectorCopy (new_velocity, ent->v.velocity);
		else
		{
			// Go along the crease
			if (numplanes != 2)
			{
				Math_VectorCopy(g_mvOrigin3f, ent->v.velocity);
				return 7;
			}

			Math_CrossProduct(planes[0], planes[1], dir);
			d = Math_DotProduct(dir, ent->v.velocity);
			Math_VectorScale(dir, d, ent->v.velocity);
		}

		// if original velocity is against the original velocity, stop dead
		// to avoid tiny occilations in sloping corners
		if(Math_DotProduct(ent->v.velocity,primal_velocity) <= 0)
		{
			Math_VectorCopy(g_mvOrigin3f,ent->v.velocity);
			return blocked;
		}
	}

	return blocked;
}

/*
===============================================================================

PUSHMOVE

===============================================================================
*/



void SV_PushMove (ServerEntity_t *pusher, float movetime)
{
	int					i, e;
	ServerEntity_t		*check, *block;
	MathVector3f_t		mins, maxs, move;
	MathVector3f_t		entorig, pushorig;
	int					num_moved;
	ServerEntity_t		**moved_edict; //johnfitz -- dynamically allocate
	MathVector3f_t		*moved_from; //johnfitz -- dynamically allocate
	int					mark; //johnfitz

	if(!pusher->v.velocity[0] && !pusher->v.velocity[1] && !pusher->v.velocity[2])
	{
		pusher->v.ltime += movetime;
		return;
	}

	for (i=0 ; i<3 ; i++)
	{
		move[i] = pusher->v.velocity[i] * movetime;
		mins[i] = pusher->v.absmin[i] + move[i];
		maxs[i] = pusher->v.absmax[i] + move[i];
	}

	Math_VectorCopy (pusher->v.origin, pushorig);

// move the pusher to it's final position

	Math_VectorAdd (pusher->v.origin, move, pusher->v.origin);
	pusher->v.ltime += movetime;
	SV_LinkEdict(pusher,false);

	//johnfitz -- dynamically allocate
	mark = Hunk_LowMark ();
	moved_edict = (ServerEntity_t**)Hunk_Alloc(sv.num_edicts*sizeof(ServerEntity_t*));
	moved_from	= (MathVector3f_t(*))Hunk_Alloc (sv.num_edicts*sizeof(MathVector3f_t));
	//johnfitz

// see if any solid entities are inside the final position
	num_moved = 0;
	check = NEXT_EDICT(sv.edicts);
	for (e=1 ; e<sv.num_edicts ; e++, check = NEXT_EDICT(check))
	{
		if (check->free)
			continue;
		if (check->v.movetype == MOVETYPE_PUSH
		|| check->v.movetype == MOVETYPE_NONE
		|| check->v.movetype == MOVETYPE_NOCLIP)
			continue;

	// if the entity is standing on the pusher, it will definately be moved
		if(!((check->v.flags & FL_ONGROUND)	&& check->v.groundentity == pusher))
		{
			if ( check->v.absmin[0] >= maxs[0]
			|| check->v.absmin[1] >= maxs[1]
			|| check->v.absmin[2] >= maxs[2]
			|| check->v.absmax[0] <= mins[0]
			|| check->v.absmax[1] <= mins[1]
			|| check->v.absmax[2] <= mins[2] )
				continue;

		// see if the ent's bbox is inside the pusher's final position
			if (!SV_TestEntityPosition (check))
				continue;
		}

	// remove the onground flag for non-players
		if (check->v.movetype != MOVETYPE_WALK)
			check->v.flags = check->v.flags & ~FL_ONGROUND;

		Math_VectorCopy(check->v.origin,entorig);
		Math_VectorCopy(check->v.origin,moved_from[num_moved]);
		moved_edict[num_moved] = check;
		num_moved++;

		// try moving the contacted entity
		pusher->Physics.iSolid = SOLID_NOT;
		Game->Physics_PushEntity(check, move);
		pusher->Physics.iSolid = SOLID_BSP;

	// if it is still inside the pusher, block
		block = SV_TestEntityPosition (check);
		if (block)
		{	// fail the move
			if (check->v.mins[0] == check->v.maxs[0])
				continue;
			if (check->Physics.iSolid == SOLID_NOT || check->Physics.iSolid == SOLID_TRIGGER)
			{	// corpse
				check->v.mins[0] = check->v.mins[1] = 0;
				Math_VectorCopy(check->v.mins,check->v.maxs);
				continue;
			}

			Math_VectorCopy(entorig,check->v.origin);
			SV_LinkEdict(check,true);

			Math_VectorCopy(pushorig,pusher->v.origin);
			SV_LinkEdict(pusher,false);
			pusher->v.ltime -= movetime;

			// if the pusher has a "blocked" function, call it
			// otherwise, just stay in place until the obstacle is gone
			if (pusher->v.BlockedFunction)
			{
				pr_global_struct.self	= ServerEntity_tO_PROG(pusher);
				pr_global_struct.eOther	= check;

				pusher->v.BlockedFunction(pusher,check);
			}

		// move back any entities we already moved
			for (i=0 ; i<num_moved ; i++)
			{
				Math_VectorCopy(moved_from[i],moved_edict[i]->v.origin);
				SV_LinkEdict (moved_edict[i], false);
			}
			Hunk_FreeToLowMark (mark); //johnfitz
			return;
		}
	}

	Hunk_FreeToLowMark (mark); //johnfitz
}

// [18/5/2013] TODO: Merge with SV_PushMove ~hogsy
static void Server_PushRotate(ServerEntity_t *pusher,float movetime)
{
	int				i,e,num_moved,slaves_moved;
	ServerEntity_t	*check,*block,*moved_edict[MAX_EDICTS],*ground,*slave,*master;
	MathVector3f_t	move,a,amove,entorig,pushorig,moved_from[MAX_EDICTS],org,org2,forward,right,up;
	bool			bMoveIt;

	for (i = 0; i < 3; i++)
		amove[i] = pusher->v.avelocity[i] * movetime;

	Math_VectorNegate(amove,a);
	Math_AngleVectors(a,forward,right,up);

	Math_VectorCopy(pusher->v.angles,pushorig);

	// move the pusher to it's final position
	Math_VectorAdd(pusher->v.angles,amove,pusher->v.angles);

	pusher->v.ltime += movetime;
	SV_LinkEdict (pusher, false);

	slaves_moved = 0;
	master = pusher;
	while(master->v.aiment)
	{
		slave = PROG_TO_EDICT(master->v.aiment);

		slaves_moved++;
		Math_VectorCopy (slave->v.angles, moved_from[MAX_EDICTS - slaves_moved]);
		moved_edict[MAX_EDICTS - slaves_moved] = slave;

		if (slave->v.movedir[PITCH])
			slave->v.angles[PITCH] = master->v.angles[PITCH];
		else
			slave->v.angles[PITCH] += slave->v.avelocity[PITCH] * movetime;

		if (slave->v.movedir[YAW])
			slave->v.angles[YAW] = master->v.angles[YAW];
		else
			slave->v.angles[YAW] += slave->v.avelocity[YAW] * movetime;

		if (slave->v.movedir[ROLL])
			slave->v.angles[ROLL] = master->v.angles[ROLL];
		else
			slave->v.angles[ROLL] += slave->v.avelocity[ROLL] * movetime;

		slave->v.ltime = master->v.ltime;
		SV_LinkEdict(slave,false);

		master = slave;
	}

	// see if any solid entities are inside the final position
	num_moved = 0;
	check = NEXT_EDICT(sv.edicts);
	for (e = 1; e < sv.num_edicts; e++, check = NEXT_EDICT(check))
	{
		if (check->free)
			continue;
		if (check->v.movetype == MOVETYPE_PUSH || check->v.movetype == MOVETYPE_NONE || check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		// if the entity is standing on the pusher, it will definitely be moved
		bMoveIt = false;
		ground = check->v.groundentity;
		if(check->v.flags & FL_ONGROUND)
		{
			if (ground == pusher)
				bMoveIt = true;
			else
			{
				for (i = 0; i < slaves_moved; i++)
				{
					if (ground == moved_edict[MAX_EDICTS - i - 1])
					{
						bMoveIt = true;
						break;
					}
				}
			}
		}

		if(!bMoveIt)
		{
			if(	check->v.absmin[0] >= pusher->v.absmax[0]
			||	check->v.absmin[1] >= pusher->v.absmax[1]
			||	check->v.absmin[2] >= pusher->v.absmax[2]
			||	check->v.absmax[0] <= pusher->v.absmin[0]
			||	check->v.absmax[1] <= pusher->v.absmin[1]
			||	check->v.absmax[2] <= pusher->v.absmin[2])
			{
				for (i = 0; i < slaves_moved; i++)
				{
					slave = moved_edict[MAX_EDICTS-i-1];
					if( check->v.absmin[0] >= slave->v.absmax[0]
					||	check->v.absmin[1] >= slave->v.absmax[1]
					||	check->v.absmin[2] >= slave->v.absmax[2]
					||	check->v.absmax[0] <= slave->v.absmin[0]
					||	check->v.absmax[1] <= slave->v.absmin[1]
					||	check->v.absmax[2] <= slave->v.absmin[2] )
						continue;
				}
				if (i == slaves_moved)
					continue;
			}

			// See if the ent's bbox is inside the pusher's final position
			if(!SV_TestEntityPosition(check))
				continue;
		}

		// remove the onground flag for non-players
		if(check->v.movetype != MOVETYPE_WALK)
			check->v.flags = check->v.flags & ~FL_ONGROUND;

		Math_VectorCopy(check->v.origin,entorig);
		Math_VectorCopy(check->v.origin,moved_from[num_moved]);
		moved_edict[num_moved] = check;
		num_moved++;

		// calculate destination position
		Math_VectorSubtract(check->v.origin,pusher->v.origin,org);
		org2[0] = Math_DotProduct(org,forward);
		org2[1] = -Math_DotProduct(org,right);
		org2[2] = Math_DotProduct(org,up);
		Math_VectorSubtract (org2,org,move);

		check->v.angles[YAW] += pusher->v.avelocity[YAW] * movetime;

		// try moving the contacted entity
		pusher->Physics.iSolid = SOLID_NOT;
		Game->Physics_PushEntity(check, move);
		pusher->Physics.iSolid = SOLID_BSP;

		// If it is still inside the pusher, block
		block = SV_TestEntityPosition (check);
		if (block)
		{
			// fail the move
			if (check->v.mins[0] == check->v.maxs[0])
				continue;
			if (check->Physics.iSolid == SOLID_NOT || check->Physics.iSolid == SOLID_TRIGGER)
			{
				// corpse
				check->v.mins[0] = check->v.mins[1] = 0;
				Math_VectorCopy(check->v.mins,check->v.maxs);
				continue;
			}

			Math_VectorCopy(entorig,check->v.origin);
			SV_LinkEdict(check,true);

			Math_VectorCopy(pushorig,pusher->v.angles);
			SV_LinkEdict(pusher,false);
			pusher->v.ltime -= movetime;

			for(i = 0; i < slaves_moved; i++)
			{
				slave = moved_edict[MAX_EDICTS - i - 1];
				Math_VectorCopy(moved_from[MAX_EDICTS - i - 1], slave->v.angles);
				SV_LinkEdict(slave,false);
				slave->v.ltime -= movetime;
			}

			// if the pusher has a "blocked" function, call it
			// otherwise, just stay in place until the obstacle is gone
			if (pusher->v.BlockedFunction)
				pusher->v.BlockedFunction(pusher,check);

			// move back any entities we already moved
			for (i = 0; i < num_moved; i++)
			{
				Math_VectorCopy (moved_from[i], moved_edict[i]->v.origin);
			//@@TODO:: see above
			//	if (!((int)moved_edict[i]->v.flags & (FL_CLIENT | FL_MONSTER)))
				Math_VectorSubtract (moved_edict[i]->v.angles, amove, moved_edict[i]->v.angles);
				moved_edict[i]->v.angles[YAW] -= amove[YAW];

				SV_LinkEdict(moved_edict[i],false);
			}
			return;
		}
	}
}

void SV_Physics_Pusher (ServerEntity_t *ent)
{
	float thinktime,oldltime,movetime;

	oldltime = ent->v.ltime;

	thinktime = ent->v.dNextThink;
	if (thinktime < ent->v.ltime + host_frametime)
	{
		movetime = thinktime-ent->v.ltime;
		if (movetime < 0)
			movetime = 0;
	}
	else
		movetime = host_frametime;

	if(movetime)
	{
		if((ent->v.avelocity[0] || ent->v.avelocity[1] || ent->v.avelocity[2])
			&& ent->Physics.iSolid == SOLID_BSP)
			Server_PushRotate(ent,movetime);
		else
			SV_PushMove(ent,movetime);	// advances ent->v.ltime if not blocked
	}

	if (thinktime > oldltime && thinktime <= ent->v.ltime)
	{
		ent->v.dNextThink = 0;

		pr_global_struct.self	= ServerEntity_tO_PROG(ent);
		pr_global_struct.eOther	= sv.edicts;

		if(ent->v.think)
			ent->v.think(ent);

		if(ent->free)
			return;
	}
}

/*
===============================================================================

CLIENT MOVEMENT

===============================================================================
*/

/*	This is a big hack to try and fix the rare case of getting stuck in the world
	clipping hull.
*/
void SV_CheckStuck (ServerEntity_t *ent)
{
	int				i,j,z;
	MathVector3f_t	org;

	if(!SV_TestEntityPosition(ent))
	{
		Math_VectorCopy(ent->v.origin,ent->v.oldorigin);
		return;
	}

	Math_VectorCopy(ent->v.origin,org);
	Math_VectorCopy(ent->v.oldorigin,ent->v.origin);
	if (!SV_TestEntityPosition(ent))
	{
		//Con_DPrintf("Unstuck.\n");
		SV_LinkEdict(ent,true);
		return;
	}

	for(z = 0; z < 18; z++)
		for (i=-1 ; i <= 1 ; i++)
			for (j=-1 ; j <= 1 ; j++)
			{
				ent->v.origin[0] = org[0]+i;
				ent->v.origin[1] = org[1]+j;
				ent->v.origin[2] = org[2]+z;

				if(!SV_TestEntityPosition(ent))
				{
					//Con_DPrintf("Unstuck.\n");
					SV_LinkEdict(ent,true);
					return;
				}
			}

	Math_VectorCopy(org,ent->v.origin);
}

/*	Player has come to a dead stop, possibly due to the problem with limited
	float precision at some angle joins in the BSP hull.

	Try fixing by pushing one pixel in each direction.

	This is a hack, but in the interest of good gameplay...
*/
int SV_TryUnstick (ServerEntity_t *ent, MathVector3f_t oldvel)
{
	int				i;
	MathVector3f_t	oldorg;
	MathVector3f_t	dir;
	int				clip;
	trace_t			steptrace;

	Math_VectorCopy (ent->v.origin, oldorg);
	Math_VectorCopy (g_mvOrigin3f, dir);

	for (i=0 ; i<8 ; i++)
	{
// try pushing a little in an axial direction
		switch (i)
		{
			case 0:
				dir[0] = 2;
				dir[1] = 0;
				break;
			case 1:
				dir[0] = 0;
				dir[1] = 2;
				break;
			case 2:
				dir[0] = -2;
				dir[1] = 0;
				break;
			case 3:
				dir[0] = 0;
				dir[1] = -2;
				break;
			case 4:
				dir[0] = 2;
				dir[1] = 2;
				break;
			case 5:	dir[0] = -2; dir[1] = 2; break;
			case 6:	dir[0] = 2; dir[1] = -2; break;
			case 7:	dir[0] = -2; dir[1] = -2; break;
		}

		Game->Physics_PushEntity(ent, dir);

		// Retry the original move
		ent->v.velocity[0] = oldvel[0];
		ent->v.velocity[1] = oldvel[1];
		ent->v.velocity[2] = 0;
		clip = SV_FlyMove(ent,0.1f,&steptrace);

		if ( fabs(oldorg[1] - ent->v.origin[1]) > 4
		|| fabs(oldorg[0] - ent->v.origin[0]) > 4 )
			return clip;

		// Go back to the original pos and try again
		Math_VectorCopy (oldorg, ent->v.origin);
	}

	Math_VectorCopy(g_mvOrigin3f,ent->v.velocity);
	return 7;		// still not moving
}

/*	Only used by players
*/
void SV_WalkMove(ServerEntity_t *ent)
{
	MathVector3f_t	upmove,downmove,oldorg,oldvel,nosteporg,nostepvel;
	int				clip,oldonground;
	trace_t			steptrace,downtrace;

	// Do a regular slide move unless it looks like you ran into a step
	oldonground = ent->v.flags & FL_ONGROUND;
	ent->v.flags = ent->v.flags & ~FL_ONGROUND;

	Math_VectorCopy (ent->v.origin, oldorg);
	Math_VectorCopy (ent->v.velocity, oldvel);

	clip = SV_FlyMove (ent, host_frametime, &steptrace);
	if(!(clip & 2))
		return;		// move didn't block on a step
	else if(!oldonground && ent->v.waterlevel == 0)
		return;		// don't stair up while jumping
	else if(ent->v.movetype != MOVETYPE_WALK)
		return;		// gibbed by a trigger
	else if(cvPhysicsNoStep.value || (sv_player->v.flags & FL_WATERJUMP))
		return;

	Math_VectorCopy (ent->v.origin, nosteporg);
	Math_VectorCopy (ent->v.velocity, nostepvel);

	// Try moving up and forward to go up a step
	Math_VectorCopy (oldorg, ent->v.origin);	// back to start pos

	Math_VectorCopy (g_mvOrigin3f, upmove);
	Math_VectorCopy (g_mvOrigin3f, downmove);

	upmove[2]	= cvPhysicsStepSize.value;
	downmove[2] = -cvPhysicsStepSize.value + oldvel[2]*host_frametime;

	// Move up
	Game->Physics_PushEntity(ent, upmove);	// FIXME: don't link?

// move forward
	ent->v.velocity[0]	= oldvel[0];
	ent->v.velocity[1]	= oldvel[1];
	ent->v.velocity[2]	= 0;
	clip = SV_FlyMove (ent, host_frametime, &steptrace);

// check for stuckness, possibly due to the limited precision of floats
// in the clipping hulls
	if (clip)
		if ( fabs(oldorg[1] - ent->v.origin[1]) < 0.03125
		&& fabs(oldorg[0] - ent->v.origin[0]) < 0.03125 )
			// stepping up didn't make any progress
			clip = SV_TryUnstick (ent, oldvel);

// extra friction based on view angle
	if ( clip & 2 )
		Game->Physics_WallFriction(ent, &steptrace);

// move down
	downtrace = Game->Physics_PushEntity(ent,downmove);	// FIXME: don't link?
	if (downtrace.plane.normal[2] > 0.7)
	{
		if (ent->Physics.iSolid == SOLID_BSP)
		{
			ent->v.flags		= ent->v.flags | FL_ONGROUND;
			ent->v.groundentity = downtrace.ent;
		}
	}
	else
	{
// if the push down didn't end up on good ground, use the move without
// the step up.  This happens near wall / slope combinations, and can
// cause the player to hop up higher on a slope too steep to climb
		Math_VectorCopy (nosteporg, ent->v.origin);
		Math_VectorCopy (nostepvel, ent->v.velocity);
	}
}

/*	Player character actions
*/
void SV_Physics_Client (ServerEntity_t	*ent, int num)
{
	if(!svs.clients[num-1].active)
		return;		// unconnected slot

	// call standard client pre-think
	pr_global_struct.self = ServerEntity_tO_PROG(ent);

	Game->Game_Init(SERVER_PLAYERPRETHINK,ent,sv.time);

	Game->Physics_CheckVelocity(ent);

	// decide which move function to call
	switch(ent->v.movetype)
	{
	case MOVETYPE_NONE:
		if (!Server_RunThink(ent))
			return;
		break;
	case MOVETYPE_WALK:
		if(!Server_RunThink(ent))
			return;

		if(!Game->Physics_CheckWater(ent) && !(ent->v.flags & FL_WATERJUMP))
			Game->Physics_SetGravity(ent);

		SV_CheckStuck (ent);
		SV_WalkMove (ent);
		break;
	case MOVETYPE_TOSS:
	case MOVETYPE_BOUNCE:
	case MOVETYPE_FLYBOUNCE:
		Physics_Toss(ent);
		break;
	case MOVETYPE_FLY:
		if(!Server_RunThink(ent))
			return;

		SV_FlyMove(ent,host_frametime,NULL);
		break;
	case MOVETYPE_NOCLIP:
		if(!Server_RunThink(ent))
			return;
		Math_VectorMA (ent->v.origin, host_frametime, ent->v.velocity, ent->v.origin);
		break;
	default:
		Sys_Error ("SV_Physics_client: bad movetype %i", (int)ent->v.movetype);
	}

	// Call standard player post-think
	SV_LinkEdict(ent,true);

	pr_global_struct.self = ServerEntity_tO_PROG(ent);

	Game->Game_Init(SERVER_CLIENTPOSTTHINK,ent,sv.time);
}

/*	A moving object that doesn't obey physics
*/
void Physics_NoClip(ServerEntity_t *eEntity)
{
	// Regular thinking
	if(!Server_RunThink(eEntity))
		return;

	Math_VectorMA(eEntity->v.angles,host_frametime,eEntity->v.avelocity,eEntity->v.angles);
	Math_VectorMA(eEntity->v.origin,host_frametime,eEntity->v.velocity,eEntity->v.origin);

	SV_LinkEdict(eEntity,false);
}

/*	Toss, bounce, and fly movement.  When onground, do nothing.
*/
void Physics_Toss(ServerEntity_t *ent)
{
	trace_t			trace;
	MathVector3f_t	move;
	float			backoff;

	// Regular thinking
	if(!Server_RunThink(ent) || (ent->v.flags & FL_ONGROUND))
		return;

	Game->Physics_CheckVelocity(ent);

	// Add gravity
	if(ent->v.movetype != MOVETYPE_FLY
	&& ent->v.movetype != MOVETYPE_FLYMISSILE
	&& ent->v.movetype != MOVETYPE_FLYBOUNCE)
		Game->Physics_SetGravity(ent);

	// Move angles
	Math_VectorMA(ent->v.angles,host_frametime,ent->v.avelocity,ent->v.angles);

	// Move origin
	Math_VectorScale(ent->v.velocity,host_frametime,move);
	trace = Game->Physics_PushEntity(ent, move);
	if(trace.fraction == 1.0f || ent->free)
		return;

	if(ent->v.movetype == MOVETYPE_FLYBOUNCE)
		backoff = 2.0f;
	else if (ent->v.movetype == MOVETYPE_BOUNCE)
		backoff = 1.5f;
	else
		backoff = 1.0f;

	ClipVelocity (ent->v.velocity, trace.plane.normal, ent->v.velocity, backoff);

	// Stop if on ground
	if(trace.plane.normal[2] > 0.7 || (ent->v.movetype != MOVETYPE_BOUNCE && ent->v.movetype != MOVETYPE_FLYBOUNCE))
		if(ent->v.velocity[2] < 60.0f)
		{
			ent->v.flags		= ent->v.flags | FL_ONGROUND;
			ent->v.groundentity = trace.ent;

			Math_VectorCopy(g_mvOrigin3f,ent->v.velocity);
			Math_VectorCopy(g_mvOrigin3f,ent->v.avelocity);
		}

	Game->Physics_CheckWaterTransition(ent);
}

/*	Monsters freefall when they don't have a ground entity, otherwise
	all movement is done with discrete steps.

	This is also used for objects that have become still on the ground, but
	will fall if the floor is pulled out from under them.
*/
void Physics_Step(ServerEntity_t *ent)
{
	// Freefall if not onground
	if(!(ent->v.flags & (FL_ONGROUND|FL_FLY|FL_SWIM)))
	{
#if 0
		// [19/3/2013] TODO: Replace! ~hogsy
		if(ent->v.velocity[2] < Cvar_VariableValue("server_gravityamount")*-0.1f)	//sv_gravity.value*-0.1f)
			bHitSound = true;
#endif

		Game->Physics_SetGravity(ent);
		Game->Physics_CheckVelocity(ent);

		SV_FlyMove(ent,host_frametime,NULL);
		SV_LinkEdict(ent,true);
	}

	// Regular thinking
	Server_RunThink(ent);

	Game->Physics_CheckWaterTransition(ent);
}

extern cvar_t	sv_edgefriction;

void Physics_AddFriction(ServerEntity_t *eEntity, MathVector3f_t vVelocity, MathVector3f_t vOrigin)
{
	float			*vel;
	float			speed, newspeed, control;
	MathVector3f_t	start, stop;
	float			friction;
	trace_t			trace;

	vel = vVelocity;

	speed = sqrt(vel[0]*vel[0] +vel[1]*vel[1]);
	if(!speed)
		return;

	// If the leading edge is over a dropoff, increase friction
	start[0] = stop[0] = vOrigin[0] + vel[0]/speed*16.0f;
	start[1] = stop[1] = vOrigin[1] + vel[1]/speed*16.0f;
	start[2] = vOrigin[2] + eEntity->v.mins[2];
	stop[2] = start[2] - 34;

	trace = SV_Move(start,g_mvOrigin3f,g_mvOrigin3f,stop,true,eEntity);
	if(trace.fraction == 1.0f)
		friction = eEntity->Physics.fFriction*sv_edgefriction.value;
	else
		friction = eEntity->Physics.fFriction;

	// Apply friction
	control = speed < cvPhysicsStopSpeed.value ? cvPhysicsStopSpeed.value : speed;
	newspeed = speed - host_frametime*control*friction;

	if(newspeed < 0)
		newspeed = 0;
	newspeed /= speed;

	Math_VectorScale(vel,newspeed,vel);
}

//============================================================================

void Physics_ServerFrame(void)
{
	int	i;
	ServerEntity_t	*eEntity;

	// Let the progs know that a new frame has started
	pr_global_struct.self	= ServerEntity_tO_PROG(sv.edicts);
	pr_global_struct.eOther	= sv.edicts;

	// TODO: should we pass the time to this? ~hogsy
	Game->Server_StartFrame();

	// Treat each object in turn
	eEntity = sv.edicts;
	for(i = 0; i < sv.num_edicts; i++,eEntity = NEXT_EDICT(eEntity))
	{
		if(eEntity->free)
			continue;

		if(pr_global_struct.force_retouch)
			SV_LinkEdict(eEntity,true);	// Force retouch even for stationary

		// [11/7/2013] Cleaned this up and gave it its own function ~hogsy
		Game->Server_EntityFrame(eEntity);

		if(i > 0 && i <= svs.maxclients)
			SV_Physics_Client(eEntity,i);
		else
		{
			switch(eEntity->v.movetype)
			{
			case MOVETYPE_NONE:
				Server_RunThink(eEntity);
				break;
			case MOVETYPE_PUSH:
				SV_Physics_Pusher(eEntity);
				break;
			case MOVETYPE_NOCLIP:
				Physics_NoClip(eEntity);
				break;
			case MOVETYPE_WALK:
			case MOVETYPE_STEP:
				Game->Physics_CheckVelocity(eEntity);
				Game->Physics_SetGravity(eEntity);

				Physics_AddFriction(eEntity,eEntity->v.velocity,eEntity->v.origin);

				SV_WalkMove(eEntity);
				SV_LinkEdict(eEntity,true);

				Server_RunThink(eEntity);
				break;
			case MOVETYPE_TOSS:
			case MOVETYPE_BOUNCE:
			case MOVETYPE_FLY:
			case MOVETYPE_FLYMISSILE:
			case MOVETYPE_FLYBOUNCE:
				Physics_Toss(eEntity);
				break;
			default:
				Con_Warning("Bad movetype set for entity! (%s)",eEntity->v.cClassname);
			}
		}
	}

	if(pr_global_struct.force_retouch)
		pr_global_struct.force_retouch--;

	sv.time += host_frametime;
}
