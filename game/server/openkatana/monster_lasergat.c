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

#include "server_weapon.h"
#include "server_item.h"

enum
{
	COMMAND_ATTACK_WAIT,	// Delay before checking for a target (and thus attacking).

	COMMAND_IDLE_WAIT,		// Delay before switching to idle from attack.

	LASERGAT_COMMAND_LOOK_DISTANCE,		// Distance of travel before stopping.
	LASERGAT_COMMAND_LOOK_PITCH,		// Should we look up or down?
	LASERGAT_COMMAND_LOOK_YAW,			// Should we look left or right?
	LASERGAT_COMMAND_LOOK_WAIT			// Delay until we start turning again.
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
	MathVector3f_t	vOrigin;

	Math_VectorSubtract(eLaserGat->v.origin,eTarget->v.origin,vOrigin);
	plVectorNormalize(vOrigin);
	Math_MVToVector(plVectorToAngles(vOrigin), eLaserGat->v.angles);
	Math_VectorInverse(eLaserGat->v.angles);
}

void LaserGat_HandleAim(ServerEntity_t *eLaserGat)
{
	if (eLaserGat->Monster.think == AI_THINK_IDLE)
	{
		if (!eLaserGat->Monster.eTarget)
		{
			if (eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_PITCH] == LASERGAT_LOOK_DOWN)
				eLaserGat->v.angles[0] -= 2;
			else if (eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_YAW] == LASERGAT_LOOK_UP)
				eLaserGat->v.angles[0] += 2;

			if (eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_YAW] == LASERGAT_LOOK_LEFT)
				eLaserGat->v.angles[1] += 2;
			else if (eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_YAW] == LASERGAT_LOOK_RIGHT)
				eLaserGat->v.angles[1] -= 2;
		}
		else
			LaserGat_AimTarget(eLaserGat, eLaserGat->Monster.eTarget);
	}
	else if (eLaserGat->Monster.think == AI_THINK_ATTACKING)
		LaserGat_AimTarget(eLaserGat, eLaserGat->Monster.eEnemy);

	// Constrain its angles...

	if (eLaserGat->v.angles[0] > 10.0f)
		eLaserGat->v.angles[0] = 10.0f;
	else if (eLaserGat->v.angles[0] < -60.0f)
		eLaserGat->v.angles[0] = -60.0f;

	if (eLaserGat->v.angles[1] > 180)
		eLaserGat->v.angles[1] = 180;
	else if (eLaserGat->v.angles[1] < -180)
		eLaserGat->v.angles[1] = -180;
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

void LaserGat_Die(ServerEntity_t *eLaserGat, ServerEntity_t *eOther, EntityDamageType_t type)
{
	eLaserGat->v.think			= LaserGat_Explode;
	eLaserGat->v.nextthink		= Server.time + 5.0;
}

void LaserGat_Think(ServerEntity_t *eLaserGat)
{
	switch (eLaserGat->Monster.think)
	{
	case AI_THINK_IDLE:
		eLaserGat->Monster.eTarget = Monster_GetTarget(eLaserGat);
		if (eLaserGat->Monster.eTarget)
		{
			eLaserGat->Monster.eOldTarget	= eLaserGat->Monster.eTarget;
			eLaserGat->Monster.eTarget		= NULL;
		}

		if (eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_WAIT] <= 0)
			eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_WAIT] = 15;
		else
		{
			if (rand() % 2 == 1)
				eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_PITCH] = LASERGAT_LOOK_UP;
			else
				eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_PITCH] = LASERGAT_LOOK_DOWN;

			if (rand() % 2 == 1)
				eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_YAW] = LASERGAT_LOOK_LEFT;
			else
				eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_YAW] = LASERGAT_LOOK_RIGHT;

			eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_WAIT]--;
		}
		break;
	case AI_THINK_ATTACKING:
		{
			if(!eLaserGat->Monster.eEnemy)
			{
				AI_SetThink(eLaserGat, AI_THINK_IDLE);
				return;
			}
			else if(!eLaserGat->Monster.eEnemy->v.health)
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
		AI_SetThink(eLaserGat, AI_THINK_IDLE);
	}

	// [22/4/2014] Make sure our pitch doesn't go crazy, heh ~hogsy
	LaserGat_HandleAim(eLaserGat);
}

void LaserGat_BasePain(ServerEntity_t *eBase, ServerEntity_t *eOther, EntityDamageType_t type)
{
	Sound(eBase,CHAN_BODY,PHYSICS_SOUND_METAL2,255,ATTN_STATIC);
}

void LaserGat_BaseDie(ServerEntity_t *eBase, ServerEntity_t *eOther, EntityDamageType_t type)
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
			(float)eBase->v.health,
			eBase->local.eOwner->v.avelocity);

		Monster_Killed(eBase->local.eOwner, eOther, type);
	}
}

void LaserGat_Spawn(ServerEntity_t *eLaserGat)
{
	Server_PrecacheModel(LASERGAT_MODEL_BASE);
	Server_PrecacheModel(LASERGAT_MODEL_BROKEN);
	Server_PrecacheModel(LASERGAT_MODEL_HEAD);

	// Get our weapon.
	Item_AddInventory(Item_GetItem(ITEM_WEAPON_LASERS), eLaserGat);

	eLaserGat->v.netname		= "Laser Turret";
	eLaserGat->v.movetype		= MOVETYPE_NONE;
	eLaserGat->v.health		= 150;
	eLaserGat->v.takedamage	= true;

	Entity_SetKilledFunction(eLaserGat, LaserGat_Die);

//	eLaserGat->Monster.PainFunction		= LaserGat_Pain;
	eLaserGat->Monster.Frame	= LaserGat_Think;
	eLaserGat->Monster.type	= MONSTER_LASERGAT;

	// Make this random so not all turrets start looking in the same directions.
	eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_PITCH] = rand() % 2;
	eLaserGat->Monster.commands[LASERGAT_COMMAND_LOOK_YAW] = rand() % 2;

	// Set our physical properties.
	eLaserGat->Physics.solid	= SOLID_BBOX;
	eLaserGat->Physics.gravity	= cv_server_gravity.value;
	eLaserGat->Physics.mass		= 3.5f;

	Entity_SetModel(eLaserGat, LASERGAT_MODEL_HEAD);
	Entity_SetSize(eLaserGat, -6.20f, -18.70f, -8.0f, 19.46f, 18.71f, 7.53f);

	AI_SetState(eLaserGat, AI_STATE_AWAKE);
	AI_SetThink(eLaserGat, AI_THINK_IDLE);

	// Now set up the base...
	ServerEntity_t	*eBase = Entity_Spawn();
	if(eBase)
	{
		eBase->v.classname		= "lasergat_base";
		eBase->v.health		= 100;
		eBase->v.movetype		= MOVETYPE_NONE;
		eBase->v.takedamage	= true;

		// Physical properties
		eBase->Physics.solid		= SOLID_BBOX;
		eBase->Physics.gravity		= 0;
		eBase->Physics.friction		= 0;
		eBase->Physics.mass			= 0;
		eBase->Physics.ignore		= eLaserGat;

		Entity_SetKilledFunction(eBase, LaserGat_BaseDie);
		Entity_SetDamagedFunction(eBase, LaserGat_BasePain);

		Entity_SetModel(eBase,LASERGAT_MODEL_BASE);
		Entity_SetSize(eBase,-22.70f,-19.60f,-18.70f,19.66f,19.28f,3.71f);
		Entity_SetAngles(eBase, eLaserGat->v.angles);
		Entity_SetOrigin(eBase,eLaserGat->v.origin);

		eLaserGat->v.origin[2] -= 30.0f;

		eLaserGat->local.eOwner	= eBase;
		eBase->local.eOwner		= eLaserGat;
	}
}
