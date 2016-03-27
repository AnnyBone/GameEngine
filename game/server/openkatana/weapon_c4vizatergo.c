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

#include "server_player.h"
#include "server_physics.h"

#define C4VIZATERGO_MAX_RANGE	700

EntityFrame_t C4Animation_Deploy[] =
{
	{   NULL, 22, 0.08f    },
	{   NULL, 23, 0.08f    },
	{   NULL, 24, 0.08f    },
	{   NULL, 25, 0.08f    },
	{   NULL, 26, 0.08f    },
	{   NULL, 27, 0.08f    },
	{   NULL, 28, 0.08f    },
	{   NULL, 29, 0.08f    },
	{   NULL, 30, 0.08f    },
	{   NULL, 31, 0.08f    },
	{   NULL, 32, 0.08f    },
	{   NULL, 33, 0.08f    },
	{   NULL, 34, 0.08f    },
	{   NULL, 35, 0.08f    },
	{   NULL, 36, 0.08f    },
	{   NULL, 37, 0.08f    },
	{   NULL, 38, 0.08f    },
	{   NULL, 39, 0.08f    },
	{   NULL, 40, 0.08f    },
	{   NULL, 41, 0.08f    },
	{   NULL, 42, 0.08f    },
	{   NULL, 43, 0.08f    },
	{   NULL, 44, 0.08f    },
	{   NULL, 45, 0.08f    },
	{   NULL, 46, 0.08f    },
	{   NULL, 47, 0.08f    },
	{   NULL, 48, 0.08f    },
	{   NULL, 49, 0.08f    },
	{   NULL, 50, 0.08f    },
	{   NULL, 51, 0.08f    },
	{   NULL, 51, 0.08f    },
	{   NULL, 52, 0.08f    },
	{   NULL, 53, 0.08f    },
	{   NULL, 54, 0.08f    },
	{   NULL, 55, 0.08f    },
	{   NULL, 56, 0.08f    },
	{   NULL, 57, 0.08f    },
	{   NULL, 58, 0.08f    },
	{   NULL, 59, 0.08f, true},
};

EntityFrame_t C4Animation_Fire1 [] =
{
	{   NULL, 1, 0.08f    },
	{   NULL, 2, 0.08f    },
	{   NULL, 3, 0.08f    },
	{   NULL, 4, 0.08f    },
	{   NULL, 5, 0.08f    },
	{   NULL, 6, 0.08f    },
	{   NULL, 7, 0.08f, true    }
};

EntityFrame_t C4Animation_Fire2 [] =
{
	{   NULL, 8, 0.08f    },
	{   NULL, 9, 0.08f    },
	{   NULL, 10, 0.08f    },
	{   NULL, 11, 0.08f    },
	{   NULL, 12, 0.08f    },
	{   NULL, 13, 0.08f    },
	{   NULL, 14, 0.08f, true    }
};

EntityFrame_t C4Animation_Fire3 [] =
{
	{   NULL, 15, 0.08f    },
	{   NULL, 16, 0.08f    },
	{   NULL, 17, 0.08f    },
	{   NULL, 18, 0.08f    },
	{   NULL, 19, 0.08f    },
	{   NULL, 20, 0.08f    },
	{   NULL, 21, 0.08f, true    }
};

EntityFrame_t C4Animation_Trigger [] =
{
	{   NULL, 40, 0.08f    },
	{   NULL, 41, 0.08f    },
	{   NULL, 42, 0.08f    },
	{   NULL, 43, 0.08f    },
	{   NULL, 44, 0.08f    },
	{   NULL, 45, 0.08f    },
	{   NULL, 46, 0.08f    },
	{   NULL, 47, 0.08f    },
	{   NULL, 48, 0.08f    },
	{   NULL, 49, 0.08f    },
	{   NULL, 50, 0.08f    },
	{   NULL, 51, 0.08f    },
	{   NULL, 52, 0.08f    },
	{   NULL, 53, 0.08f    },
	{   NULL, 54, 0.08f    },
	{   NULL, 55, 0.08f    },
	{   NULL, 56, 0.08f    },
	{   NULL, 57, 0.08f    },
	{   NULL, 58, 0.08f    },
	{   NULL, 59, 0.08f, true    }
};

void C4Vizatergo_Deploy(ServerEntity_t *ent)
{
	Sound(ent,CHAN_WEAPON,"weapons/c4/c4cock.wav",255,ATTN_NORM);

	Weapon_Animate(ent,C4Animation_Deploy);
}

void C4Vizatergo_Explode(ServerEntity_t *eVizatergo)
{
	Sound(eVizatergo,CHAN_AUTO,va("fx/explosion%i.wav",rand()%6+1),255,ATTN_NORM);

	Entity_RadiusDamage(eVizatergo,MONSTER_RANGE_NEAR,40,DAMAGE_TYPE_EXPLODE);
	Entity_Remove(eVizatergo);
}

void C4Vizatergo_C4BallTouch(ServerEntity_t *ent,ServerEntity_t *eOther)
{
#if 0
	// Don't touch our owner.
	if(other == ent->local.eOwner)
		return;

	// Play the stick sound!
	Sound(ent,CHAN_ITEM,"weapons/c4/c4stick.wav",255,ATTN_NORM);

	if(other->Physics.iSolid == SOLID_BSP && ent->v.movetype != MOVETYPE_NONE)
		ent->v.movetype = MOVETYPE_NONE;
	else
	{
		ent->v.TouchFunction	= NULL;
		ent->v.enemy			= other;
		ent->v.think			= WEAPON_StickThink;
		ent->v.dNextThink		= Server.time;
	}
#else	// [12/12/2012] New method. Explode if touches monster / another player. ~hogsy
	if(	eOther != ent->local.eOwner && eOther->Monster.iType)
		C4Vizatergo_Explode(ent);
	else if(eOther->Physics.iSolid == SOLID_BSP && ent->v.movetype != MOVETYPE_NONE)
	{
		// Play the stick sound!
		Sound(ent,CHAN_ITEM,"weapons/c4/c4stick.wav",255,ATTN_NORM);

		ent->v.movetype	= MOVETYPE_NONE;

		ent->local.hit = true;
	}
#endif
}

void C4Vizatergo_Think(ServerEntity_t *ent)
{
#if 0	// TODO: Finish?
	float			fDistance;
	plVector3f_t	vDistance;
	ServerEntity_t	*eDistantEnt = Game.Server_FindRadius(ent->v.origin,512.0f);

	if(eDistantEnt)
	{
		Math_VectorSubtract(eDistantEnt->v.origin,ent->v.origin,vDistance);
		fDistance = 512.0f - plLengthf(vDistance);

		Game.Con_Printf("Distance: %f\n",fDistance);
		if(fDistance > 0)
			Sound(ent,CHAN_AUTO,"weapons/c4/c4beep.wav",255,ATTN_NORM);
	}
	else
		fDistance = 0;

	ent->v.dNextThink = Server.dTime+(2.5f*(fDistance/100.0f));
#else
	Sound(ent, CHAN_AUTO, "weapons/c4/beep.wav", 130, ATTN_IDLE);

	if(!ent->local.eOwner && ent->local.hit)
		ent->v.movetype = MOVETYPE_BOUNCE;

	ent->v.dNextThink = Server.dTime+2.5;
#endif
}

void C4Vizatergo_PrimaryAttack(ServerEntity_t *eOwner)
{
	MathVector3f_t vOrigin;
	MathVector3f_t mvDirection;
	ServerEntity_t *c4ball = Entity_Spawn();

	Sound(eOwner,CHAN_AUTO,"weapons/c4/c4fire.wav",255,ATTN_NORM);
	Sound(eOwner,CHAN_AUTO,"weapons/c4/c4cock.wav",255,ATTN_NORM);

	Weapon_Animate(eOwner,C4Animation_Fire1);
	Weapon_ViewPunch(eOwner, 7, true);

	eOwner->v.iPrimaryAmmo = eOwner->local.iC4Ammo -= 1;

	c4ball->v.cClassname	= "c4ball";
	c4ball->v.movetype		= MOVETYPE_BOUNCE;

	c4ball->local.style = AMMO_C4BOMBS;		// Cleaner way to tell if this can explode or not :V ~hogsy
	c4ball->local.iC4Ammo = 1;				// [11/8/2013] Since style is used for other shit too LAWL ~hogsy
	c4ball->local.eOwner = eOwner;

	// Set the physical properties.
	c4ball->Physics.iSolid = SOLID_BBOX;
	c4ball->Physics.fMass = 0.9f;
	c4ball->Physics.eIgnore = eOwner;
	c4ball->Physics.fGravity = SERVER_GRAVITY;

	Math_MVToVector(Weapon_Aim(eOwner), mvDirection);
	Math_VectorScale(mvDirection, C4VIZATERGO_MAX_RANGE, c4ball->v.velocity);

	c4ball->v.velocity[PL_Y] += 20.0f;

	Math_MVToVector(plVectorToAngles(c4ball->v.velocity), c4ball->v.angles);
	Math_VectorCopy(eOwner->v.origin,vOrigin);

	c4ball->v.TouchFunction = C4Vizatergo_C4BallTouch;
	c4ball->v.think = C4Vizatergo_Think;
	c4ball->v.dNextThink = Server.dTime + 2.5;

	Entity_SetModel(c4ball,"models/c4ammo.md2");
	Entity_SetSizeVector(c4ball, pl_origin3f, pl_origin3f);
	Entity_SetOrigin(c4ball,vOrigin);

	if(eOwner->local.attackb_finished > Server.dTime)	// No attack boost...
		eOwner->local.dAttackFinished = Server.dTime+0.6;
	else
		eOwner->local.dAttackFinished = Server.dTime+1.2;
}

void C4Vizatergo_SecondaryAttack(ServerEntity_t *eOwner)
{
	ServerEntity_t *eExplodable = Engine.Server_FindRadius(eOwner->v.origin,10000.0f);

	do
	{
		if((eExplodable->local.style == AMMO_C4BOMBS) && (eExplodable->local.iC4Ammo == 1) && (eExplodable->local.eOwner == eOwner))
		{
			eExplodable->v.think		= C4Vizatergo_Explode;
			eExplodable->v.dNextThink	= Server.dTime;
		}

		eExplodable = eExplodable->v.chain;
	} while(eExplodable);

	Weapon_Animate(eOwner,C4Animation_Trigger);
}
