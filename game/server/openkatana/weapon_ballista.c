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

ServerEntityFrame_t BallistaAnimation_Deploy[] =
{
	{   NULL, 35, 0.1f    },
	{   NULL, 36, 0.1f    },
	{   NULL, 37, 0.1f    },
	{   NULL, 38, 0.1f    },
	{   NULL, 39, 0.1f, TRUE    }
};

void Ballista_Deploy(ServerEntity_t *ent)
{
	Weapon_Animate(ent,BallistaAnimation_Deploy);
}

void Ballista_LogTouch(ServerEntity_t *ent, ServerEntity_t *other)
{
	char snd[64];

	if(other == ent->local.owner)
		return;

	// We hit an enemy! Stick with 'em
	if(other->v.takedamage)
	{
		sprintf(snd,"weapons/ballista/logwetimpact%i.wav",rand()%4);

		Entity_Damage(other, ent, 25, 0);

		ent->v.think		= WEAPON_StickThink;
		ent->v.nextthink	= Server.time+0.1;
	}
	// We didn't hit anything, so NOW we set up our impact sound.
	else
		sprintf(snd,"weapons/ballista/logimpact%i.wav",rand()%5);

	Sound(ent,CHAN_ITEM,snd,255,ATTN_NORM);

    plClearVector3D(&ent->v.velocity);
    plClearVector3D(&ent->v.avelocity);

	ent->v.enemy = other;
}

void Ballista_SpawnLogProjectile(ServerEntity_t *ent)
{
#if 0 // todo
	ServerEntity_t *log = Entity_Spawn();

	log->local.owner = ent;

	log->v.movetype = MOVETYPE_FLY;
	log->Physics.solid = SOLID_BBOX;

	Weapon_Projectile(ent, log, 2000.0f);

	Entity_SetModel(log,"models/log.md2");
	Entity_SetSizeVector(log, plCreateVector3D(0, 0, 0), plCreateVector3D(0, 0, 0));

	log->v.origin = ent->v.origin;
	log->v.origin.z += 15.0f;

	Math_MVToVector(plVectorToAngles(log->v.velocity), log->v.angles);

	log->v.TouchFunction = Ballista_LogTouch;
#endif
}

void Ballista_PrimaryAttack(ServerEntity_t *ent)
{
	//if (ent->v.waterlevel >= 2)
	//	Sound(ent,CHAN_WEAPON,"weapons/ballista/cbwaterfire.wav",255,ATTN_NORM);
	//else
	//	Sound(ent,CHAN_WEAPON,"weapons/ballista/cbfire.wav",255,ATTN_NORM);

    Weapon_ViewPunch(ent, 5, true);

	ent->v.primary_ammo	= ent->local.ballista_ammo--;

	Ballista_SpawnLogProjectile(ent);

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+1.0;
	else
		ent->local.dAttackFinished = Server.time+2.0;
}
