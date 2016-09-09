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

#include "server_weapon.h"
#include "server_effects.h"

ServerEntityFrame_t efPulseRifleDeploy[] =
{
	{ NULL, 1, 0.1f, false },
	{ NULL, 2, 0.1f, false },
	{ NULL, 3, 0.1f, false },
	{ NULL, 4, 0.1f, false },
	{ NULL, 5, 0.1f, false },
	{ NULL, 6, 0.1f, false },
	{ NULL, 7, 0.1f, false },
	{ NULL, 8, 0.1f, false },
	{ NULL, 9, 0.1f, false },
	{ NULL, 10, 0.1f, false },
	{ NULL, 11, 0.1f, false },
	{ NULL, 12, 0.1f, false },
	{ NULL, 13, 0.1f, false },
	{ NULL, 14, 0.1f, false },
	{ NULL, 15, 0.1f, true }
};

ServerEntityFrame_t efPulseRifleAttack[] =
{
	{ NULL, 16, 0.1f, false },
	{ NULL, 17, 0.1f, false },
	{ NULL, 18, 0.1f, false },
	{ NULL, 19, 0.1f, false },
	{ NULL, 20, 0.1f, false },
	{ NULL, 21, 0.1f, false },
	{ NULL, 22, 0.1f, false },
	{ NULL, 23, 0.1f, false },
	{ NULL, 24, 0.1f, false },
	{ NULL, 25, 0.1f, false },
	{ NULL, 26, 0.1f, false },
	{ NULL, 27, 0.1f, true }
};

void PulseRifle_Deploy(ServerEntity_t *ent)
{
	Weapon_Animate(ent,efPulseRifleDeploy);
}

void PulseRifle_PrimaryAttack(ServerEntity_t *ent)
{
	Sound(ent,CHAN_WEAPON,"weapons/pulserifle/pulsefire2.wav",255,ATTN_NORM);

#if 0
	if(rand()%2 == 1)
		//Weapon_Animate(ent,FALSE,0,15,0.05f,10,19,0,FALSE);
	else
		//Weapon_Animate(ent,FALSE,16,27,0.05f,10,19,0,FALSE);
#else
	Weapon_Animate(ent,efPulseRifleAttack);
#endif

	ServerEffect_MuzzleFlash(ent->v.origin, ent->v.angles);

	ent->v.iPrimaryAmmo	= ent->local.glock_ammo--;
	ent->local.glock_ammo2--;

	ent->local.dAttackFinished	= Server.dTime+0.2;

	// [23/5/2012] Damage set to 5 due to high rate of fire ~hogsy
	//Weapon_BulletProjectile(ent,0.5f,5);
}

// [17/7/2012] Revised ~hogsy
void CorditeExplode(ServerEntity_t *ent)
{
	PLVector3f vel;

	// [25/6/2012] Simplified ~hogsy
	Math_VectorCopy(ent->v.velocity,vel);
	Math_VectorInverse(vel);

	Engine.Particle(ent->v.origin,vel,5,"spark",17);

	// [25/6/2012] Simplified ~hogsy
	plVectorClear(ent->v.velocity);

	Entity_Remove(ent);
}

// [17/7/2012] Revised ~hogsy
void CorditeTouch(ServerEntity_t *ent, ServerEntity_t *other)
{
	if(other == ent->local.eOwner)
		return;

	CorditeExplode(ent);

	Entity_Damage(other, ent, 50, 0);
}

void throw_cordite(ServerEntity_t *ent)
{
	ServerEntity_t *greekfire = Entity_Spawn();

	greekfire->v.cClassname	= "cordite";
	greekfire->v.movetype	= MOVETYPE_BOUNCE;

	greekfire->Physics.solid	= SOLID_BBOX;

	greekfire->local.eOwner = ent;

	Weapon_Projectile(ent, greekfire, 2000.0f);

	Entity_SetModel(greekfire,"models/grenade.mdl");

	Math_MVToVector(plVectorToAngles(greekfire->v.velocity), greekfire->v.angles);

	Entity_SetSize(greekfire,0,0,0,0,0,0);

	greekfire->v.think			= CorditeExplode;
	greekfire->v.dNextThink		= Server.dTime + 3.0;
	greekfire->v.TouchFunction	= CorditeTouch;

	// SetOrigin automatically links.
	Entity_SetOrigin(greekfire, ent->v.origin);
}

void PulseRifle_SecondaryAttack(ServerEntity_t *ent)
{
	// [23/5/2012] Revised ~hogsy
	// [25/6/2012] Revised ~hogsy
	char *noise;

	if(ent->local.dAttackFinished > Server.dTime)
		return;

	switch(ent->local.iFireMode)
	{
	case 1:
		ent->local.iFireMode = 0;
		noise = "weapons/pulserifle/pulsechange2.wav";
		break;
	default:
		ent->local.iFireMode = 1;
		noise = "weapons/pulserifle/pulsechange1.wav";
	}

	Sound(ent,CHAN_WEAPON,noise,255,ATTN_NORM);

	ent->local.dAttackFinished = Server.dTime+1.0;
}
