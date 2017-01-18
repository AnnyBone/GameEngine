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

ServerEntityFrame_t AxeAnimation_Deploy[] =
{
	{   NULL, 2, 0.1f    },
	{   NULL, 3, 0.1f    },
	{   NULL, 4, 0.1f    },
	{   NULL, 5, 0.1f    },
	{   NULL, 6, 0.1f		},
	{   NULL, 7, 0.1f, true }
};

ServerEntityFrame_t AxeAnimation_Attack1[] =
{
	{   NULL, 4, 0.05f    },
	{   NULL, 5, 0.05f    },
	{   NULL, 6, 0.05f    },
	{   NULL, 7, 0.05f    },
	{   NULL, 8, 0.05f    },
	{   NULL, 9, 0.05f    },
	{   NULL, 10, 0.05f    },
	{   NULL, 11, 0.05f    },
	{   NULL, 12, 0.05f    },
	{   NULL, 13, 0.05f    },
	{   NULL, 14, 0.05f    },
	{   NULL, 15, 0.05f    },
	{   NULL, 16, 0.05f, TRUE    }
};

ServerEntityFrame_t AxeAnimation_Attack2[] =
{
	{   NULL, 17, 0.05f    },
	{   NULL, 18, 0.05f    },
	{   NULL, 19, 0.05f    },
	{   NULL, 20, 0.05f    },
	{   NULL, 21, 0.05f    },
	{   NULL, 22, 0.05f    },
	{   NULL, 23, 0.05f    },
	{   NULL, 24, 0.05f    },
	{   NULL, 25, 0.05f    },
	{   NULL, 26, 0.05f    },
	{   NULL, 27, 0.05f, TRUE    }
};

void Axe_Deploy(ServerEntity_t *ent)
{
	Weapon_Animate(ent,AxeAnimation_Deploy);
}

void AxeHit(ServerEntity_t *ent)
{
	PLVector3f forward,temp,sndvec,vel;
	trace_t	trace;

	plAngleVectors(ent->v.v_angle, forward, temp, temp);

	// [18/4/2012] A nice soft bounce ~hogsy
	vel[0] = vel[1] = 0;
	vel[2] = 0.5;

	sndvec[0] = ent->v.origin[0]+forward[0]*64;
	sndvec[1] = ent->v.origin[1]+forward[1]*64;
	sndvec[2] = ent->v.origin[2]+forward[2]*64;

	trace = Traceline(ent,ent->v.origin,sndvec,0);

	sndvec[0] = trace.endpos[0]-forward[0]*4;
	sndvec[1] = trace.endpos[1]-forward[1]*4;
	sndvec[2] = trace.endpos[2]-forward[2]*4;

	if(trace.fraction == 1.0f)
		return;
	if(trace.ent->v.bTakeDamage)
	{
		if(trace.ent->local.bBleed)
			Engine.Particle(sndvec,vel,10,"blood",30);

		Entity_Damage(trace.ent, ent, 20, 0);
	}
	else if(trace.ent)
	{
		Engine.Particle(sndvec,vel,10,"smoke",30);
	}

}

void Axe_PrimaryAttack(ServerEntity_t *ent)
{

	Sound(ent,CHAN_WEAPON,"weapons/axe/axeswing.wav",255,ATTN_NORM);

	if(rand()%2 == 1)
		Weapon_Animate(ent,AxeAnimation_Attack1);
	else
		Weapon_Animate(ent,AxeAnimation_Attack2);

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+0.25;
	else
		ent->local.dAttackFinished = Server.time+0.5;
}
