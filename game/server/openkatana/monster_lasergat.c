/*	Copyright (C) 2011-2015 OldTimes Software

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

#include "server_weapon.h"
#include "server_item.h"

enum
{
	COMMAND_ATTACK_WAIT,	// Delay before checking for a target (and thus attacking).

	COMMAND_IDLE_WAIT,		// Delay before switching to idle from attack.

	COMMAND_LOOK_PITCH,		// Should we look up or down?
	COMMAND_LOOK_YAW,		// Should we look left or right?
	COMMAND_LOOK_WAIT		// Delay until we start turning again.
};

#define LASERGAT_LOOK_RIGHT	0	// Look right.
#define	LASERGAT_LOOK_LEFT	1	// Look left.
#define	LASERGAT_LOOK_DOWN	0	// Look down.
#define	LASERGAT_LOOK_UP	1	// Look up.
#define LASERGAT_LOOK_NONE	2	// Don't do either.

#define	LASERGAT_MAX_WAIT	rand()%50

Weapon_t *wLaserWeapon;

void LaserGat_AimTarget(ServerEntity_t *eLaserGat,ServerEntity_t *eTarget)
{
	vec3_t	vOrigin;

	Math_VectorSubtract(eLaserGat->v.origin,eTarget->v.origin,vOrigin);
	Math_VectorNormalize(vOrigin);
	Math_MVToVector(Math_VectorToAngles(vOrigin),eLaserGat->v.angles);
	Math_VectorInverse(eLaserGat->v.angles);
}

void LaserGat_HandleAim(ServerEntity_t *eLaserGat)
{
	if(eLaserGat->Monster.iThink == THINK_IDLE)
	{
		if(!eLaserGat->Monster.eTarget)
		{
		}
		else
			LaserGat_AimTarget(eLaserGat,eLaserGat->Monster.eTarget);
	}
	else if(eLaserGat->Monster.iThink == THINK_ATTACKING)
		LaserGat_AimTarget(eLaserGat,eLaserGat->Monster.eEnemy);
}

void LaserGat_Touch(ServerEntity_t *eLaserGat,ServerEntity_t *eOther)
{
	// TODO: Explode
	Sound(eLaserGat,CHAN_BODY,PHYSICS_SOUND_METAL2,255,ATTN_STATIC);
}

void LaserGat_Explode(ServerEntity_t *eLaserGat)
{
	Sound(eLaserGat,CHAN_AUTO,va("fx/explosion%i.wav",rand()%6+1),255,ATTN_NORM);

	Entity_RadiusDamage(eLaserGat,300.0f,25,DAMAGE_TYPE_EXPLODE);
	Entity_Remove(eLaserGat);
}

void LaserGat_Die(ServerEntity_t *eLaserGat,ServerEntity_t *eOther)
{
	eLaserGat->v.think		= LaserGat_Explode;
	eLaserGat->v.dNextThink	= Server.dTime + 5.0;
}

void LaserGat_Think(ServerEntity_t *eLaserGat)
{
	switch(eLaserGat->Monster.iThink)
	{
	case THINK_IDLE:
		// Check if there are any targets nearby...
		if(!eLaserGat->Monster.iCommandList[COMMAND_ATTACK_WAIT])
		{
			eLaserGat->Monster.eTarget = Monster_GetTarget(eLaserGat);
			if(eLaserGat->Monster.eTarget)
			{
#if 0
				// [11/6/2013] Try setting the target as an enemy... ~hogsy
				if (Monster_GetRelationship(eLaserGat, eLaserGat->Monster.eTarget) == MONSTER_RELATIONSHIP_HATE)
				{
					// [6/4/2013] Set think and state for next frame ~hogsy
					Monster_SetThink(eLaserGat,THINK_ATTACKING);
					Monster_SetState(eLaserGat,STATE_AWAKE);
					return;
				}
#endif
				{
					eLaserGat->Monster.eOldTarget	= eLaserGat->Monster.eTarget;
					eLaserGat->Monster.eTarget		= NULL;
				}
			}
			// [6/4/2013] TODO: Check if it's neutral and worth damaging? ~hogsy

			eLaserGat->Monster.iCommandList[COMMAND_ATTACK_WAIT] = 5;
		}
		else
			eLaserGat->Monster.iCommandList[COMMAND_ATTACK_WAIT]--;

		if(!eLaserGat->Monster.iCommandList[COMMAND_LOOK_WAIT])
		{
			if(eLaserGat->Monster.iCommandList[COMMAND_LOOK_PITCH] == LASERGAT_LOOK_DOWN)
				eLaserGat->v.angles[0] -= 0.5f;
			else if(eLaserGat->Monster.iCommandList[COMMAND_LOOK_YAW] == LASERGAT_LOOK_UP)
				eLaserGat->v.angles[0] += 0.5f;

			if(eLaserGat->Monster.iCommandList[COMMAND_LOOK_YAW] == LASERGAT_LOOK_LEFT)
				eLaserGat->v.angles[1] += 0.5f;
			else if(eLaserGat->Monster.iCommandList[COMMAND_LOOK_YAW] == LASERGAT_LOOK_RIGHT)
				eLaserGat->v.angles[1] -= 0.5f;

			if(rand()%125 == 1)
			{
				eLaserGat->Monster.iCommandList[COMMAND_LOOK_WAIT] = LASERGAT_MAX_WAIT;
				if(rand()%2 == 1)
					eLaserGat->Monster.iCommandList[COMMAND_LOOK_PITCH]	= LASERGAT_LOOK_UP;
				else
					eLaserGat->Monster.iCommandList[COMMAND_LOOK_PITCH]	= LASERGAT_LOOK_DOWN;

				if(rand()%2 == 1)
					eLaserGat->Monster.iCommandList[COMMAND_LOOK_YAW]	= LASERGAT_LOOK_LEFT;
				else
					eLaserGat->Monster.iCommandList[COMMAND_LOOK_YAW]	= LASERGAT_LOOK_RIGHT;
			}
		}
		break;
	case THINK_ATTACKING:
		{
			if(!eLaserGat->Monster.eEnemy)
			{
				Monster_SetThink(eLaserGat,THINK_IDLE);
				return;
			}
			else if(!eLaserGat->Monster.eEnemy->v.iHealth)
			{
				// [6/4/2013] TODO: Add a delay here until we actually stop shooting? Means we might gib it which would be kewl... ~hogsy
				eLaserGat->Monster.eEnemy = NULL;
				break;
			}

			// [22/4/2014] Switched over to use this instead since it manages delay etc for us :) ~hogsy
			Weapon_PrimaryAttack(eLaserGat);
		}
		break;
	default:
		Monster_SetThink(eLaserGat,THINK_IDLE);
	}

	// [22/4/2014] Make sure our pitch doesn't go crazy, heh ~hogsy
	if(eLaserGat->v.angles[0] > 10.0f)
		eLaserGat->v.angles[0] = 10.0f;
	else if(eLaserGat->v.angles[0] < -60.0f)
		eLaserGat->v.angles[0] = -60.0f;
}

void LaserGat_BasePain(ServerEntity_t *eBase,ServerEntity_t *eOther)
{
	Sound(eBase,CHAN_BODY,PHYSICS_SOUND_METAL2,255,ATTN_STATIC);
}

void LaserGat_BaseDie(ServerEntity_t *eBase,ServerEntity_t *eOther)
{
	Entity_SetModel(eBase,LASERGAT_MODEL_BROKEN);

	// Check that we still have our owner...
	if(eBase->local.eOwner)
	{
		// Clear out the owners owner.
		eBase->local.eOwner->local.eOwner = NULL;

		eBase->local.eOwner->v.movetype			= MOVETYPE_BOUNCE;
		eBase->local.eOwner->v.TouchFunction	= LaserGat_Touch;

		Math_VectorScale(
			eBase->local.eOwner->v.avelocity,
			(float)eBase->v.iHealth,
			eBase->local.eOwner->v.avelocity);

		Monster_Killed(eBase->local.eOwner,eOther);
	}
}

void LaserGat_Spawn(ServerEntity_t *eLaserGat)
{
	Server_PrecacheModel(LASERGAT_MODEL_BASE);
	Server_PrecacheModel(LASERGAT_MODEL_BROKEN);
	Server_PrecacheModel(LASERGAT_MODEL_HEAD);

	// Get our weapon.
	Item_AddInventory(Item_GetItem(WEAPON_LASERS),eLaserGat);

	eLaserGat->v.netname		= "Laser Turret";
	eLaserGat->v.movetype		= MOVETYPE_NONE;
	eLaserGat->v.iHealth		= 150;
	eLaserGat->v.bTakeDamage	= true;

	Entity_SetKilledFunction(eLaserGat, LaserGat_Die);

//	eLaserGat->Monster.PainFunction		= LaserGat_Pain;
	eLaserGat->Monster.Think			= LaserGat_Think;
	eLaserGat->Monster.fViewDistance	= MONSTER_RANGE_MEDIUM;
	eLaserGat->Monster.iType			= MONSTER_LASERGAT;

	// Make this random so not all turrets start looking in the same directions.
	eLaserGat->Monster.iCommandList[COMMAND_LOOK_PITCH]	= rand()%2;
	eLaserGat->Monster.iCommandList[COMMAND_LOOK_YAW]	= rand()%2;

	// Set our physical properties.
	eLaserGat->Physics.iSolid		= SOLID_BBOX;
	eLaserGat->Physics.fGravity		= cvServerGravity.value;
	eLaserGat->Physics.fMass		= 3.5f;

	Entity_SetModel(eLaserGat,LASERGAT_MODEL_HEAD);
	Entity_SetSize(eLaserGat,-6.20f,-18.70f,-8.0f,19.46f,18.71f,7.53f);

	Monster_SetState(eLaserGat,STATE_AWAKE);
	Monster_SetThink(eLaserGat,THINK_IDLE);

	{
		// Now set up the base...
		ServerEntity_t	*eBase = Entity_Spawn();
		if(eBase)
		{
			eBase->v.cClassname = "lasergat_base";
			eBase->v.iHealth = 100;
			eBase->v.movetype = MOVETYPE_NONE;
			eBase->v.bTakeDamage = true;

			// Physical properties
			eBase->Physics.iSolid = SOLID_BBOX;
			eBase->Physics.fGravity = 0;
			eBase->Physics.fFriction = 0;
			eBase->Physics.fMass = 0;

			Entity_SetKilledFunction(eBase, LaserGat_BaseDie);
			Entity_SetDamagedFunction(eBase, LaserGat_BasePain);

			Entity_SetModel(eBase,LASERGAT_MODEL_BASE);
			Entity_SetSize(eBase,-22.70f,-19.60f,-18.70f,19.66f,19.28f,3.71f);
			Entity_SetAngles(eBase, eLaserGat->v.angles);
			Entity_SetOrigin(eBase,eLaserGat->v.origin);

			eLaserGat->v.origin[2] -= 30.0f;

			eLaserGat->local.eOwner	= eBase;
			eBase->local.eOwner	= eLaserGat;
			eBase->Physics.eIgnore = eLaserGat;
		}
	}
}
