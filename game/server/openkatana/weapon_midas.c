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

void Midas_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,7,0.1f,0,0,0,FALSE);
}

void Midas_CloudThink(ServerEntity_t *ent)
{
	if (ent->local.hit == 0)
	{
		Entity_Remove(ent);
		return;
	}

	ent->local.hit = ent->local.hit - 2;

	Engine.Particle(ent->v.origin, pl_origin3f, 12.0f, "spark", 8);

	ent->v.dNextThink	= Server.dTime+0.3;
	ent->v.think		= Midas_CloudThink;
}

void CloudTouch(ServerEntity_t *cloud, ServerEntity_t *other)
{
	if(Entity_IsPlayer(other))
		return;

	if(other->v.iHealth > 0 && other->v.movetype == MOVETYPE_STEP)
	{
		if(!other->local.hit) // Only play it once
			Sound(other,CHAN_VOICE,"weapons/midastouch.wav",255,ATTN_NORM);

		other->v.dNextThink = Server.dTime+15.0;

		other->local.hit = 1;
	}
	else
		// We probably hit a wall so don't hang about!
		Entity_Remove(cloud);
}

void Midas_PrimaryAttack(ServerEntity_t *ent)
{
	ServerEntity_t *cloud = Entity_Spawn();

	cloud->v.cClassname	= "cloud";
	cloud->v.movetype	= MOVETYPE_FLYMISSILE;

	cloud->Physics.iSolid = SOLID_TRIGGER;

	cloud->local.hit	= 10;
	cloud->local.eOwner = ent;

//	SetSize(cloud,-16,-16,-16,16,16,16);
	Entity_SetOrigin(cloud,ent->v.origin);

	Weapon_Projectile(ent, cloud, 100.0f);

	cloud->v.dNextThink		= Server.dTime+0.3;
	cloud->v.think			= Midas_CloudThink;
	cloud->v.TouchFunction	= CloudTouch;

	Math_VectorAddValue(cloud->v.avelocity,300.0f,cloud->v.avelocity);

	if(ent->local.attackb_finished > Server.dTime)	// No attack boost...
		ent->local.dAttackFinished = Server.dTime+0.5;
	else
		ent->local.dAttackFinished = Server.dTime+1.0;

	//Weapon_Animate(ent,FALSE,8,20,0.5,0,0,0,FALSE);
}
