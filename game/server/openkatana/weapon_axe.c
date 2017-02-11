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
	PLVector3D forward,sndvec,vel;
	trace_t	trace;

	Math_AngleVectors(ent->v.v_angle, &forward, NULL, NULL);

	// [18/4/2012] A nice soft bounce ~hogsy
	vel.x = vel.y = 0;
	vel.z = 0.5f;

	sndvec.x = ent->v.origin.x+forward.x*64;
	sndvec.y = ent->v.origin.y+forward.y*64;
	sndvec.z = ent->v.origin.z+forward.z*64;

	trace = Traceline(ent,ent->v.origin,sndvec,0);

	sndvec.x = trace.endpos.x-forward.x * 4;
	sndvec.y = trace.endpos.y-forward.y * 4;
	sndvec.z = trace.endpos.z-forward.z * 4;

	if(trace.fraction == 1.0f)
		return;

	if(trace.ent->v.takedamage) {
		if(trace.ent->local.bleed)
			Engine.Particle(sndvec,vel,10,"blood",30);

		Entity_Damage(trace.ent, ent, 20, DAMAGE_TYPE_NORMAL);
	} else if(trace.ent) {
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
