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

void Shockwave_SpawnProjectile(ServerEntity_t *ent);

ServerEntityFrame_t ShockwaveAnimation_Deploy[] =
{
	{	NULL, 19, 0.075f},
	{	NULL, 20, 0.075f},
	{	NULL, 21, 0.075f},
	{	NULL, 22, 0.075f},
	{	NULL, 23, 0.075f},
	{	NULL, 24, 0.075f},
	{	NULL, 25, 0.075f},
	{	NULL, 26, 0.075f},
	{	NULL, 27, 0.075f},
	{	NULL, 28, 0.075f},
	{	NULL, 29, 0.075f, TRUE},
};

ServerEntityFrame_t ShockwaveAnimation_FireA[] =
{
	{	NULL, 1, 0.075f},
	{	NULL, 2, 0.075f},
	{	NULL, 3, 0.075f},
	{	NULL, 4, 0.075f},
	{	NULL, 5, 0.075f},
	{	NULL, 6, 0.075f, TRUE},
};

ServerEntityFrame_t ShockwaveAnimation_FireB[] =
{
	{	NULL, 7, 0.075f},
	{	NULL, 8, 0.075f},
	{	NULL, 9, 0.075f},
	{	NULL, 10, 0.075f},
	{	NULL, 11, 0.075f},
	{	NULL, 12, 0.075f, TRUE},
};

ServerEntityFrame_t ShockwaveAnimation_FireC[] =
{
	{	NULL, 13, 0.075f},
	{	NULL, 14, 0.075f},
	{	NULL, 15, 0.075f},
	{	NULL, 16, 0.075f},
	{	NULL, 17, 0.075f},
	{	NULL, 18, 0.075f, TRUE},
};


void Shockwave_Deploy(ServerEntity_t *ent)
{
	Weapon_Animate(ent,ShockwaveAnimation_Deploy);
	Sound(ent,CHAN_WEAPON,"weapons/shockwave/ready.wav",255,ATTN_NORM);
}

void ShockLaser_Touch(ServerEntity_t *ent, ServerEntity_t *other)
{
	char	*cSound;

	if(!other || (other == ent->local.eOwner))
		return;

	if(other->v.takedamage)
	{
		// burning flesh sound
		cSound = "weapons/shockwave/burn.wav";

		Entity_Damage(other, ent, 100, 0);
	}
	else
		cSound = "weapons/shockwave/fade.wav";

	Sound(ent,CHAN_ITEM,cSound,255,ATTN_NORM);

	Entity_Remove(ent);
}

void Shockwave_SpawnProjectile(ServerEntity_t *ent)
{
	ServerEntity_t *eLaser;

	Sound(ent,CHAN_WEAPON,"weapons/shockwave/fire.wav",255,ATTN_NORM);

	ent->v.punchangle[0] -= 10.0f;
	
	eLaser = Entity_Spawn();
	if(eLaser)
	{
		Weapon_Projectile(ent, eLaser, 2000.0f);

		Math_VectorCopy(ent->v.origin,eLaser->v.origin);
		Math_MVToVector(plVectorToAngles(eLaser->v.velocity),eLaser->v.angles);

		eLaser->local.eOwner = ent;

		eLaser->v.movetype		= MOVETYPE_FLY;
		eLaser->v.TouchFunction = ShockLaser_Touch;
		eLaser->v.origin[2]		+= 25.0f;

		eLaser->Physics.solid	= SOLID_BBOX;

		Entity_SetModel(eLaser,"models/slaser.md2");
		Entity_SetSizeVector(eLaser, pl_origin3f, pl_origin3f);
	}

	ent->local.shockwave_ammo--;
	ent->v.primary_ammo = ent->local.shockwave_ammo;
}

void Shockwave_PrimaryAttack(ServerEntity_t *ent)
{
	Sound(ent,CHAN_WEAPON,"weapons/shockwave/warmup.wav",255,ATTN_NORM);

	int iRand = rand()%3;
	if(iRand == 2)
		Weapon_Animate(ent,ShockwaveAnimation_FireA);
	else if(iRand == 1)
		Weapon_Animate(ent,ShockwaveAnimation_FireB);
	else
		Weapon_Animate(ent,ShockwaveAnimation_FireC);

	Shockwave_SpawnProjectile(ent);

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+0.65;
	else
		ent->local.dAttackFinished = Server.time+1.3;
}
