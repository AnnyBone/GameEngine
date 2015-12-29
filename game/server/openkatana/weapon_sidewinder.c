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

/*
	Episode One's basic rocket launcher, the SideWinder.
*/

#include "server_player.h"
#include "server_effects.h"

#define	SIDEWINDER_MAXSPEED	600.0f
#define SIDEWINDER_MINSPEED	50.0f

EntityFrame_t SideWinderAnimation_Deploy [] =
{
	{	NULL, 25, 0.02f},
	{	NULL, 26, 0.02f},
	{	NULL, 27, 0.02f},
	{	NULL, 28, 0.02f},
	{	NULL, 29, 0.02f},
	{	NULL, 30, 0.02f},
	{	NULL, 31, 0.02f},
	{	NULL, 32, 0.02f},
	{	NULL, 33, 0.02f},
	{	NULL, 34, 0.02f},
	{	NULL, 35, 0.02f},
	{	NULL, 36, 0.02f},
	{	NULL, 37, 0.02f},
	{	NULL, 38, 0.02f},
	{	NULL, 39, 0.02f},
	{	NULL, 40, 0.02f, TRUE},
};

EntityFrame_t SideWinderAnimation_Fire [] =
{
	{	NULL, 1, 0.02f},
	{	NULL, 2, 0.02f},
	{	NULL, 3, 0.02f},
	{	NULL, 4, 0.02f},
	{	NULL, 5, 0.02f},
	{	NULL, 6, 0.02f},
	{	NULL, 7, 0.02f},
	{	NULL, 8, 0.02f},
	{	NULL, 9, 0.02f},
	{	NULL, 10, 0.02f},
	{	NULL, 11, 0.02f},
	{	NULL, 12, 0.02f},
	{	NULL, 13, 0.02f},
	{	NULL, 14, 0.02f},
	{	NULL, 15, 0.02f},
	{	NULL, 16, 0.02f},
	{	NULL, 17, 0.02f},
	{	NULL, 18, 0.02f},
	{	NULL, 19, 0.02f},
	{	NULL, 20, 0.02f},
	{	NULL, 21, 0.02f},
	{	NULL, 22, 0.02f},
	{	NULL, 23, 0.02f},
	{	NULL, 24, 0.02f, TRUE},
};

void SideWinder_Think(ServerEntity_t *ent);

void SideWinder_Deploy(ServerEntity_t *ent)
{
	// TODO: Deploy sound?

	Weapon_Animate(ent,SideWinderAnimation_Deploy);
}

void SideWinder_MissileExplode(ServerEntity_t *ent,ServerEntity_t *other)
{
	MathVector3f_t vVelocity;

	if(other && (other == ent->local.eOwner))
	{
		SideWinder_Think(ent);
		return;
	}

	Entity_RadiusDamage(ent, MONSTER_RANGE_NEAR, 30, DAMAGE_TYPE_EXPLODE);

	Math_VectorNegate(ent->v.velocity,vVelocity);

	if (Engine.Server_PointContents(ent->v.origin) <= BSP_CONTENTS_WATER)
		Sound(ent, CHAN_AUTO, SOUND_EXPLODE_UNDERWATER0, 255, ATTN_NORM);
	else
		Sound(ent, CHAN_AUTO, SOUND_EXPLODE, 255, ATTN_NORM);

	ServerEffect_Explosion(ent->v.origin);

	Entity_Remove(ent);
}

void SideWinder_Think(ServerEntity_t *eSideWinder)
{
	int	iContents = Engine.Server_PointContents(eSideWinder->v.origin);
	if ((iContents == BSP_CONTENTS_WATER) || 
		(iContents == BSP_CONTENTS_SLIME) || 
		(iContents == BSP_CONTENTS_LAVA))
		eSideWinder->local.speed = SIDEWINDER_MINSPEED;
	// Don't let us explode in the sky.
	else if(iContents == BSP_CONTENTS_SKY)
	{
		Entity_Remove(eSideWinder);
		return;
	}
	else
		eSideWinder->local.speed = SIDEWINDER_MAXSPEED;

	if(eSideWinder->local.count > 12)
		eSideWinder->local.count = 0;

//	eSideWinder->v.velocity[X]	*= ((float)sin(Server.dTime*1.5f)*5.5f)/10.0f;
//	eSideWinder->v.velocity[Y]	*= -((float)sin(Server.dTime*1.5f)*5.5f)/10.0f;
//	eSideWinder->v.velocity[Z]	*= eSideWinder->v.velocity[X]/eSideWinder->v.velocity[Y];

	if(Server.dTime >= eSideWinder->local.fSpawnDelay)
	{
		SideWinder_MissileExplode(eSideWinder,NULL);
		return;
	}

	eSideWinder->v.dNextThink = Server.dTime+0.05;
}

void SideWinder_SpawnMissle(ServerEntity_t *ent,float fSpeed,float ox)
{
	MathVector3f_t vOrg;
	ServerEntity_t *eMissile = Entity_Spawn();

	// TODO: Spawn a flare at our position too

	eMissile->v.cClassname = "sidewindermissile";
	eMissile->v.movetype = MOVETYPE_FLYMISSILE;
	eMissile->v.effects = EF_PARTICLE_SMOKE | EF_DIMLIGHT;

	eMissile->Physics.iSolid	= SOLID_BBOX;
	eMissile->Physics.eIgnore	= ent;

	eMissile->local.speed	= SIDEWINDER_MAXSPEED;
	eMissile->local.eOwner	= ent;
	eMissile->local.count	= 0;
	// Change our speed depending on what contents we're within.
	eMissile->local.speed	= fSpeed;

	Weapon_Projectile(ent, eMissile, eMissile->local.speed);

	Entity_SetModel(eMissile,"models/sidewinder_missile.md2");

	// Copy the angles over so we're facing the right way.
	Math_VectorCopy(ent->v.angles, eMissile->v.angles);

	Math_VectorCopy(ent->v.origin,vOrg);

	vOrg[0] += eMissile->local.vForward[0] * 8 + eMissile->local.vRight[0] * ox;
	vOrg[1] += eMissile->local.vForward[1] * 8 + eMissile->local.vRight[1] * ox;
	vOrg[2] += eMissile->local.vForward[2] * 24;

	Entity_SetSizeVector(eMissile,g_mvOrigin3f,g_mvOrigin3f);
	Entity_SetOrigin(eMissile,vOrg);

	// Time at which we'll be removed if nothing hit.
	eMissile->local.fSpawnDelay = (float)(Server.dTime+8.0);

	eMissile->v.TouchFunction	= SideWinder_MissileExplode;
	eMissile->v.dNextThink		= Server.dTime+0.05;
	eMissile->v.think			= SideWinder_Think;

	Entity_Link(eMissile,false);
}

void SideWinder_PrimaryAttack(ServerEntity_t *eOwner)
{
	float	fSpeed	= SIDEWINDER_MAXSPEED;
	char	*cSound = SOUND_SIDEWINDER_FIRE;

	int iCurrentContents = Engine.Server_PointContents(eOwner->v.origin);
	if( (iCurrentContents == BSP_CONTENTS_WATER) || 
		(iCurrentContents == BSP_CONTENTS_SLIME) || 
		(iCurrentContents == BSP_CONTENTS_LAVA))
	{
		cSound = "weapons/sidewinder/sidewinderunderwaterfire.wav";
		fSpeed = SIDEWINDER_MINSPEED;
	}

	Sound(eOwner,CHAN_WEAPON,cSound,255,ATTN_NORM);

	Weapon_Animate(eOwner,SideWinderAnimation_Fire);

	// [1/10/2012] Readded ~hogsy
	eOwner->v.iPrimaryAmmo	= eOwner->local.sidewinder_ammo -= 2;

	eOwner->v.punchangle[PITCH] -= (float)(rand()%5+1);

	SideWinder_SpawnMissle(eOwner,fSpeed,10);
	SideWinder_SpawnMissle(eOwner,fSpeed,-10);

	eOwner->local.dAttackFinished = Server.dTime+1.2;
}
