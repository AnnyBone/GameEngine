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
			t->v.cClassname	= "DelayedUse";
			t->v.dNextThink	= Server.dTime+ent->local.delay;
			t->v.think		= Misc_DelayThink;
			t->v.enemy		= other;
		}

		return;
	}

	if (ent->local.killtarget)
	{
		t = Engine.Server_FindEntity(Server.eWorld,ent->local.killtarget,FALSE);
		if(!t)
			return;

		Entity_Remove(t);
	}

	if(ent->v.targetname)
	{
		t = Engine.Server_FindEntity(Server.eWorld,ent->v.targetname,FALSE);
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

	if(((ent->v.enemy != Server.eWorld) && (ent->v.enemy->v.iHealth > 1)))
	{
		Entity_SetOrigin(ent,ent->v.enemy->v.origin);
		if(!ent->v.velocity)
			Math_VectorClear(ent->v.avelocity);
	}
	else
		Entity_Remove(ent);

	ent->v.dNextThink = (Server.dTime+0.100);
}

// [17/7/2012] Renamed to Misc_GibThink ~hogsy
void Misc_GibThink(ServerEntity_t *ent)
{
	Entity_Remove(ent);
}

void ThrowGib(vec3_t origin,vec3_t velocity,char *model,float damage,bool bleed)
{
	ServerEntity_t *gib = Entity_Spawn();

	gib->v.cClassname	= "gib";

	Entity_SetOrigin(gib,origin);

	if(bleed)
		gib->v.effects = EF_PARTICLE_BLOOD;

	gib->v.velocity[0]	= velocity[0]*((damage+rand()%20)/2.0f);
	gib->v.velocity[1]	= velocity[1]*((damage+rand()%20)/2.0f);
	gib->v.velocity[2]	= velocity[2]*((damage+rand()%20)/2.0f);
	gib->v.movetype		= MOVETYPE_BOUNCE;

	gib->Physics.iSolid		= SOLID_NOT;
	gib->Physics.fGravity	= SERVER_GRAVITY;
	gib->Physics.fMass		= 1.0f;

	Math_VectorSet((float)(rand()%10*damage),gib->v.avelocity);

	Entity_SetModel(gib,model);
	Entity_SetSizeVector(gib,g_mvOrigin3f,g_mvOrigin3f);

	gib->v.think		= Misc_GibThink;
	gib->v.dNextThink	= Server.dTime+20.0f;
}
