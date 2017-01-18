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

void Zeus_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,19,0.08f,0,0,0,FALSE);
}

void Zeus_Hit(ServerEntity_t *ent)
{
	PLVector3f	vSource, vTarg, vel, mvDirection;
	trace_t	trace;

	Math_MVToVector(Weapon_Aim(ent), mvDirection);

	vSource[0] = ent->v.origin[0];
	vSource[1] = ent->v.origin[1];
	vSource[2] = ent->v.origin[2]+22.0f;

	vTarg[0] = vSource[0] + (mvDirection[0] * 250);
	vTarg[1] = vSource[1] + (mvDirection[1] * 250);
	vTarg[2] = vSource[2] + (mvDirection[2] * 250);

	plVectorClear(vel);

	trace = Traceline(ent,vSource,vTarg,0);
	if(trace.fraction != 1.0f)
		{
			if(trace.ent && trace.ent->v.takedamage)
			{
				if(trace.ent->local.bBleed)
					Engine.Particle(trace.endpos,vel,10,"blood",30);

				Entity_Damage(trace.ent, ent, 5, 0);
			}
			else
				Engine.Particle(trace.endpos,vel,10,"zspark",30);
		}
}

void Zeus_PrimaryAttack(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,20,35,0.08f,0,0,0,FALSE);

	if(ent->local.attackb_finished > Server.time)
		ent->local.dAttackFinished = Server.time+0.6;
	else
		ent->local.dAttackFinished = Server.time+1.2;
}
