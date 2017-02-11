/*	Copyright (C) 2011-2016 OldTimes Software

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

#include "server_weapon.h"

void Barrier_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,13,0.1f,0,0,0,FALSE);
}

#if 0   // [10/2/2014] TODO: This needs to be rewritten ~hogsy
void BarrierFire(ServerEntity_t *ent)
{
	vec3_t	forward,temp,sndvec,vel;
	trace_t	trace;
	ServerEntity_t *bshield = Entity_Spawn();

	if(ent->local.b_ent)
		Entity_Remove(ent->local.b_ent);

	ent->local.b_ent = bshield;

	plAngleVectors(ent->v.v_angle, forward, temp, temp);

	// [18/4/2012] A nice soft bounce ~hogsy
	vel[0] = vel[1] = 0;
	vel[2] = 0.5f;

	sndvec[0] = ent->v.origin[0]+forward[0]*64.0f;
	sndvec[1] = ent->v.origin[1]+forward[1]*64.0f;
	sndvec[2] = ent->v.origin[2]+forward[2]*64.0f+25.0f;

	trace = Traceline(ent,ent->v.origin,sndvec,0);
	sndvec[0] = trace.endpos[0]-forward[0]*4.0f;
	sndvec[1] = trace.endpos[1]-forward[1]*4.0f;
	sndvec[2] = trace.endpos[2]-forward[2]*4.0f;

	bshield->v.classname	= "bshield";
	bshield->v.movetype		= MOVETYPE_NONE;
	bshield->Physics.iSolid	= SOLID_BBOX;

	bshield->local.owner = ent;
	// [18/5/2012] Removed for now as EF_LIGHT_BLUE isn't defined anywhere on my side! ~hogsy
//	bshield->v.effects = (int)bshield->v.effects | EF_LIGHT_BLUE;

	Math_VectorCopy(ent->v.angles,bshield->v.angles);

	Entity_SetModel(bshield,"models/bshield.md2");
	Entity_SetSize(bshield,-1.0f,-20.0f,-25.0f,1.0f,20.0f,25.0f);
	Entity_SetOrigin(bshield, trace.endpos);

	bshield->v.think		= Entity_Remove;
	bshield->v.nextthink	= Server.time+0.5;
}

void Barrier_PrimaryAttack(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,0,15,0.05f,10,19,0,FALSE);
	BarrierFire(ent);
}
#endif
