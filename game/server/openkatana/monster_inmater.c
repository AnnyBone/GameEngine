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

#include "server_monster.h"

/*
	The Inmater
	A horrid robot that lurks around the prisons keeping
	those locked inside in line and under control.
*/

ServerEntityFrame_t efInmaterIdle[] =
{
	{ NULL, 1, 0.1f },
	{ NULL, 1, 0.1f },
	{ NULL, 2, 0.1f },
	{ NULL, 3, 0.1f },
	{ NULL, 4, 0.1f },
	{ NULL, 5, 0.1f },
	{ NULL, 6, 0.1f },
	{ NULL, 7, 0.1f },
	{ NULL, 8, 0.1f },
	{ NULL, 9, 0.1f },
	{ NULL, 10, 0.1f },
	{ NULL, 11, 0.1f },
	{ NULL, 12, 0.1f },
	{ NULL, 13, 0.1f },
	{ NULL, 14, 0.1f },
	{ NULL, 15, 0.1f },
	{ NULL, 16, 0.1f },
	{ NULL, 17, 0.1f },
	{ NULL, 18, 0.1f },
	{ NULL, 19, 0.1f },
	{ NULL, 20, 0.1f },
	{ NULL, 21, 0.1f },
	{ NULL, 22, 0.1f },
	{ NULL, 23, 0.1f, true }
};

ServerEntityFrame_t efInmaterDeath[] =
{
	{ NULL, 148, 0.1f },
	{ NULL, 149, 0.1f },
	{ NULL, 150, 0.1f },
	{ NULL, 151, 0.1f },
	{ NULL, 152, 0.1f },
	{ NULL, 153, 0.1f },
	{ NULL, 154, 0.1f },
	{ NULL, 155, 0.1f },
	{ NULL, 156, 0.1f },
	{ NULL, 157, 0.1f },
	{ NULL, 158, 0.1f },
	{ NULL, 159, 0.1f },
	{ NULL, 160, 0.1f },
	{ NULL, 161, 0.1f },
	{ NULL, 162, 0.1f },
	{ NULL, 163, 0.1f },
	{ NULL, 164, 0.1f },
	{ NULL, 165, 0.1f },
	{ NULL, 166, 0.1f },
	{ NULL, 167, 0.1f, true }
};

#define INMATER_MAX_HEALTH	150
#define	INMATER_MAX_BOREDOM	45
#define	INMATER_MAX_SPEED	120.0f

#define	INMATER_MIN_HEALTH	-35
#define	INMATER_MIN_BOREDOM	-45

void Inmater_Pain(ServerEntity_t *eInmater, ServerEntity_t *eOther, EntityDamageType_t type)
{
}

void Inmater_Die(ServerEntity_t *eInmater, ServerEntity_t *eOther, EntityDamageType_t type)
{
	if(eInmater->v.health < INMATER_MIN_HEALTH)
	{
		int	iGibs = (rand()%5)+5,
			i;

		Sound(eInmater,CHAN_VOICE,"misc/gib1.wav",255,ATTN_NORM);

		// [13/9/2012] Updated paths ~hogsy
		for(i = 0; i < iGibs; i++)
		{
			char			cModel[64];
			int				j;
			PLVector3D	vOrigin,vGibVelocity;

			vOrigin.x = eInmater->v.origin.x + ((eInmater->v.mins.x + eInmater->v.maxs.x) * (rand()%10));
			vOrigin.y = eInmater->v.origin.y + ((eInmater->v.mins.y + eInmater->v.maxs.y) * (rand()%10));
			vOrigin.z = eInmater->v.origin.z + ((eInmater->v.mins.z + eInmater->v.maxs.z) * (rand()%10));

			// [26/2/2014] Add some random velocity on top of the velocity that the inmater already has ~hogsy
            vGibVelocity = eInmater->v.velocity;
			plAddVector3Df(&vGibVelocity, (float)(rand() % 5));

			PHYSICS_MODEL_METAL(cModel);

			ThrowGib(vOrigin, vGibVelocity, cModel, (float)eInmater->v.health*-1, true);
		}

		Engine.Particle(eInmater->v.origin,eInmater->v.velocity,10.0f,"blood",20);

		Entity_Remove(eInmater);
		return;
	}

	Entity_Animate(eInmater,efInmaterDeath);

	AI_SetState(eInmater, AI_STATE_DEAD);
}

void Inmater_Think(ServerEntity_t *eInmater)
{
	switch (eInmater->Monster.state)
	{
	case AI_STATE_ASLEEP:
		//if (eInmater->Monster.meEmotion[EMOTION_BOREDOM].iEmotion > INMATER_MAX_BOREDOM)
		{
			AI_SetState(eInmater, AI_STATE_AWAKE);
			AI_SetThink(eInmater, AI_THINK_WANDERING);
			return;
		}
		break;
	case AI_STATE_AWAKE:
		switch (eInmater->Monster.think)
		{
		case AI_THINK_IDLE:
			if(!eInmater->local.animation_time || (eInmater->local.iAnimationCurrent == eInmater->local.animation_end))
				Entity_Animate(eInmater,efInmaterIdle);

			AI_RandomMovement(eInmater, INMATER_MAX_SPEED);
			break;
		}
		break;
	case AI_STATE_DEAD:
		break;
	default:
		AI_SetState(eInmater, AI_STATE_ASLEEP);
	}
}

void Inmater_Spawn(ServerEntity_t *eInmater)
{
	Server_PrecacheModel(MODEL_INMATER_BODY);

	Entity_SetPhysics(eInmater, SOLID_SLIDEBOX, 3.0f, 4.5f);

	eInmater->Monster.type = MONSTER_INMATER;
	eInmater->Monster.Frame = Inmater_Think;
	eInmater->Monster.Pain = Inmater_Pain;

	Entity_SetKilledFunction(eInmater, Inmater_Die);

	eInmater->v.takedamage = true;
	eInmater->v.movetype = MOVETYPE_STEP;
	eInmater->v.health = INMATER_MAX_HEALTH;
	eInmater->v.netname = "Inmater";
	eInmater->v.frame = 0;
	eInmater->local.maxhealth = INMATER_MAX_HEALTH;

	AI_SetState(eInmater, AI_STATE_AWAKE);
	AI_SetThink(eInmater, AI_THINK_IDLE);

	Entity_SetModel(eInmater, MODEL_INMATER_BODY);
	Entity_SetSize(eInmater, -16.0f, -16.0f, -24.0f, 16.0f, 16.0f, 32.0f);
	Entity_SetOrigin(eInmater, eInmater->v.origin);

	Entity_DropToFloor(eInmater);
}
