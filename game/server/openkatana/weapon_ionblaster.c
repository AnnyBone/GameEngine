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

/*	The Ion Blaster. Hiro's starter weapon.	*/

#include "server_player.h"

#define IONBLASTER_MAX_RANGE	2000.0f
#define	IONBLASTER_MAX_HITS		10

ServerEntityFrame_t IonBlasterAnimation_Deploy[] =
{
	{ NULL, 1, 0.02f },
	{ NULL, 2, 0.02f },
	{ NULL, 3, 0.02f },
	{ NULL, 4, 0.02f },
	{ NULL, 5, 0.02f },
	{ NULL, 6, 0.02f },
	{ NULL, 7, 0.02f },
	{ NULL, 8, 0.02f },
	{ NULL, 9, 0.02f },
	{ NULL, 10, 0.02f },
	{ NULL, 11, 0.02f },
	{ NULL, 12, 0.02f },
	{ NULL, 13, 0.02f },
	{ NULL, 14, 0.02f },
	{ NULL, 15, 0.02f },
	{ NULL, 16, 0.02f },
	{ NULL, 17, 0.02f },
	{ NULL, 18, 0.02f },
	{ NULL, 19, 0.02f },
	{ NULL, 20, 0.02f },
	{ NULL, 21, 0.02f },
	{ NULL, 22, 0.02f },
	{ NULL, 23, 0.02f },
	{ NULL, 24, 0.02f },
	{ NULL, 25, 0.02f },
	{ NULL, 26, 0.02f, TRUE }
};

ServerEntityFrame_t IonBlasterAnimation_Fire1[] =
{
	{ NULL, 27, 0.02f },
	{ NULL, 28, 0.02f },
	{ NULL, 29, 0.02f },
	{ NULL, 30, 0.02f },
	{ NULL, 31, 0.02f },
	{ NULL, 32, 0.02f },
	{ NULL, 33, 0.02f },
	{ NULL, 34, 0.02f },
	{ NULL, 35, 0.02f },
	{ NULL, 36, 0.02f },
	{ NULL, 37, 0.02f, TRUE }
};

ServerEntityFrame_t IonBlasterAnimation_Fire2[] =
{
	{ NULL, 38, 0.02f },
	{ NULL, 39, 0.02f },
	{ NULL, 40, 0.02f },
	{ NULL, 41, 0.02f },
	{ NULL, 42, 0.02f },
	{ NULL, 43, 0.02f },
	{ NULL, 44, 0.02f },
	{ NULL, 45, 0.02f },
	{ NULL, 46, 0.02f },
	{ NULL, 47, 0.02f },
	{ NULL, 48, 0.02f },
	{ NULL, 49, 0.02f },
	{ NULL, 50, 0.02f },
	{ NULL, 51, 0.02f, TRUE }
};

void IonBlaster_Deploy(ServerEntity_t *ent)
{
	Sound(ent, CHAN_WEAPON, SOUND_WEAPON_SWITCH, 255, ATTN_NORM);

	Weapon_Animate(ent,IonBlasterAnimation_Deploy);
}

void IonBlaster_IonBallExplode(ServerEntity_t *ent)
{
	MathVector3f_t vel;

	// [18/5/2013] This was the wrong way, fixed now! ~hogsy
	Math_VectorCopy(ent->v.velocity,vel);
	Math_VectorInverse(vel);

	Sound(ent,CHAN_ITEM,"weapons/ionblaster/explode.wav",255,ATTN_NORM);

	Engine.Particle(ent->v.origin,vel,5.0f,"spark2",20);

	// TODO: Fancy explosion effect with particles? ~hogsy

	Entity_Remove(ent);
}

void IonBlaster_IonBallTouch(ServerEntity_t *eIonBall, ServerEntity_t *other)
{
	MathVector3f_t vInversed;

	// Increment hit count.
	eIonBall->local.count++;

	// Ensure we aren't inside something...
	int	iPointContent = Engine.Server_PointContents(eIonBall->v.origin);
	if ((iPointContent == BSP_CONTENTS_SKY) || (iPointContent == BSP_CONTENTS_SOLID))
	{
		Entity_Remove(eIonBall);
		return;
	}

#if 0
	// Don't collide with the owner initially...
	if((other == eIonBall->local.eOwner) && !eIonBall->local.hit)
		return;
	// If we hit something else, and then hit the owner again, it'll do damage!
	else
		eIonBall->local.hit = true;
#endif

	if (eIonBall->local.count < IONBLASTER_MAX_HITS)
	{
		if (Entity_CanDamage(eIonBall, other, DAMAGE_TYPE_NORMAL) && (other != eIonBall->local.eOwner))
		{
			Entity_Damage(other, eIonBall, 15, 0);
			Entity_Remove(eIonBall);
			return;
		}

		Math_VectorCopy(eIonBall->v.velocity, vInversed);
		Math_VectorInverse(vInversed);
		Math_MVToVector(plVectorToAngles(eIonBall->v.velocity), eIonBall->v.angles);

		Sound(eIonBall, CHAN_ITEM, "weapons/ionblaster/bounce.wav", 255, ATTN_NORM);

		Engine.Particle(eIonBall->v.origin, vInversed, 5.0f, "spark2", 25);

		return;
	}

	IonBlaster_IonBallExplode(eIonBall);
}

void IonBlaster_PrimaryAttack(ServerEntity_t *ent)
{
	ServerEntity_t	*eIonBall;
	MathVector3f_t	orig;

	// Check if there's room to perform the attack.
	if (!Weapon_CheckTrace(ent))
		return;

	Sound(ent,CHAN_WEAPON,"weapons/ionblaster/fire.wav",255,ATTN_NORM);

	Weapon_ViewPunch(ent, 4, false);

	if(rand()%2 == 1)
		Weapon_Animate(ent,IonBlasterAnimation_Fire1);
	else
		Weapon_Animate(ent,IonBlasterAnimation_Fire2);

	// This fixes the ammo bug ~eukos [05/08/2013]
	ent->local.ionblaster_ammo--;
	ent->v.iPrimaryAmmo = ent->local.ionblaster_ammo;

	eIonBall = Entity_Spawn();
	if(eIonBall)
	{
		eIonBall->v.classname = "ionball";
		eIonBall->v.movetype = MOVETYPE_FLYBOUNCE;
		eIonBall->v.effects = EF_LIGHT_GREEN;

		eIonBall->Physics.solid = SOLID_BBOX;

		eIonBall->local.hit = false;
		eIonBall->local.eOwner = ent;
		eIonBall->local.count = 0;

		Weapon_Projectile(ent, eIonBall, IONBLASTER_MAX_RANGE);

		plVectorCopy(ent->v.angles,eIonBall->v.angles);
		plVectorCopy(ent->v.origin, orig);

		orig[2] += 25.0f;

		Entity_SetModel(eIonBall,"models/ionball.md2");
		Entity_SetSize(eIonBall, 0,0,0,0,0,0);
		Entity_SetOrigin(eIonBall, orig);

		eIonBall->v.TouchFunction	= IonBlaster_IonBallTouch;
		eIonBall->v.nextthink		= Server.time+3.0;
		eIonBall->v.think			= IonBlaster_IonBallExplode;

		Entity_Link(eIonBall, false);
	}

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+0.25;
	else
		ent->local.dAttackFinished = Server.time+0.5;
}
