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

void Hermes_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,11,0.1f,0,0,0,FALSE);
}

void Hermes_CloudThink(ServerEntity_t *ent)
{
	MathVector3f_t vel;

	if (!ent->local.hit)
	{
		Entity_Remove(ent);
		return;
	}

	ent->local.hit -= 2;

	Math_VectorClear(vel);

	Engine.Particle(ent->v.origin,vel,12,"poison",8);

	ent->v.think		= Hermes_CloudThink;
	ent->v.dNextThink	= Server.dTime+0.3;
}

void HermesCloudTouch(ServerEntity_t *ent, ServerEntity_t *other)
{
	if(other->Monster.iType != MONSTER_PLAYER)
		return;

	if(other->v.iHealth > 0 && other->v.movetype == MOVETYPE_STEP)
	{
		Entity_Damage(other, ent, 5, 0);
		//other->local.poisoned = 1; TODO: MAKE IT WORK
	}
}

void Hermes_PrimaryAttack(ServerEntity_t *ent)
{
	ServerEntity_t *cloud = Entity_Spawn();

	cloud->v.cClassname = "cloud";
	cloud->v.movetype	= MOVETYPE_FLYMISSILE;
	cloud->Physics.iSolid		= SOLID_TRIGGER;

	cloud->local.hit	= 10;
	cloud->local.eOwner	= ent;

	//SetSize(cloud,-16,-16,-16,16,16,16);
	Entity_SetOrigin(cloud,ent->v.origin);

	Weapon_Projectile(ent, cloud, 100.0f);

	cloud->v.dNextThink		= Server.dTime+0.3;
	cloud->v.think			= Hermes_CloudThink;
	cloud->v.TouchFunction	= HermesCloudTouch;

	Math_VectorAddValue(cloud->v.avelocity,300.0f,cloud->v.avelocity);

	if(ent->local.attackb_finished > Server.dTime)	// No attack boost...
		ent->local.dAttackFinished = Server.dTime+0.35;
	else
		ent->local.dAttackFinished = Server.dTime+0.7;

#if 0
	if(rand()%3 == 1)
		//Weapon_Animate(ent,FALSE,12,17,0.07f,10,19,0,FALSE);
	else if(rand()%3 == 2)
		//Weapon_Animate(ent,FALSE,18,23,0.07f,10,19,0,FALSE);
	else
		//Weapon_Animate(ent,FALSE,24,29,0.07f,10,19,0,FALSE);
#endif
}
