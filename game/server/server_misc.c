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

#include "server_misc.h"

void UseTargets(ServerEntity_t *ent, ServerEntity_t *other);

void Misc_DelayThink(ServerEntity_t *ent)
{
	ServerEntity_t *other = ent->v.enemy;

	UseTargets(ent, other);

	Entity_Remove(ent);
}

void UseTargets(ServerEntity_t *ent, ServerEntity_t *other)
{
	ServerEntity_t *t;

	ent->local.activator = other;

	if(ent->local.delay)
	{
		t = Entity_Spawn();
		if(t)
		{
			t->v.classname	= "DelayedUse";
			t->v.nextthink	= Server.time+ent->local.delay;
			t->v.think		= Misc_DelayThink;
			t->v.enemy		= other;
		}

		return;
	}

	if (ent->local.killtarget)
	{
		t = Engine.Server_FindEntity(Server.world,ent->local.killtarget,FALSE);
		if(!t)
			return;

		Entity_Remove(t);
	}

	if(ent->v.targetname)
	{
		t = Engine.Server_FindEntity(Server.world,ent->v.targetname,FALSE);
		if(t)
		{
			t->local.activator = other;
			if(t->v.use)
				t->v.use(t);
			return;
		}
	}
}

void WEAPON_StickThink(ServerEntity_t *ent)
{
	ent->v.think			= WEAPON_StickThink;
	ent->v.TouchFunction	= NULL;

	if(((ent->v.enemy != Server.world) && (ent->v.enemy->v.health > 1)))
	{
		Entity_SetOrigin(ent,ent->v.enemy->v.origin);
#if 0
		if(!ent->v.velocity)
			plVectorClear(ent->v.avelocity);
#endif
	}
	else
		Entity_Remove(ent);

	ent->v.nextthink = (Server.time+0.100);
}

// [17/7/2012] Renamed to Misc_GibThink ~hogsy
void Misc_GibThink(ServerEntity_t *ent)
{
	Entity_Remove(ent);
}

void ThrowGib(PLVector3D origin, PLVector3D velocity, const char *model, float damage, bool bleed) {
	ServerEntity_t *gib = Entity_Spawn();

	gib->v.classname	= "gib";

	Entity_SetOrigin(gib,origin);

	if(bleed)
		gib->v.effects = EF_PARTICLE_BLOOD;

	gib->v.velocity.x	= velocity.x*((damage+rand()%20)/2.0f);
	gib->v.velocity.y	= velocity.y*((damage+rand()%20)/2.0f);
	gib->v.velocity.z	= velocity.z*((damage+rand()%20)/2.0f);
	gib->v.movetype		= MOVETYPE_BOUNCE;

	gib->Physics.solid		= SOLID_NOT;
	gib->Physics.gravity	= SERVER_GRAVITY;
	gib->Physics.mass		= 1.0f;

    float damt = (float)(rand() % 10 * damage);
    gib->v.avelocity = plCreateVector3D(damt, damt, damt);

	Entity_SetModel(gib,model);
	Entity_SetSizeVector(gib, plCreateVector3D(0, 0, 0), plCreateVector3D(0, 0, 0));

	gib->v.think		= Misc_GibThink;
	gib->v.nextthink	= Server.time+20.0f;
}
