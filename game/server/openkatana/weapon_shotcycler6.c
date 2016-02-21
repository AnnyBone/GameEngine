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
	Episode One's shotgun
*/

#include "server_player.h"

EntityFrame_t ShotcyclerAnimation_Deploy [] =
{
	{ NULL, 31, 0.04f },
	{ NULL, 32, 0.04f },
	{ NULL, 33, 0.04f },
	{ NULL, 34, 0.04f },
	{ NULL, 35, 0.04f },
	{ NULL, 36, 0.04f },
	{ NULL, 37, 0.04f },
	{ NULL, 38, 0.04f },
	{ NULL, 39, 0.04f },
	{ NULL, 40, 0.04f },
	{ NULL, 41, 0.04f },
	{ NULL, 42, 0.04f },
	{ NULL, 43, 0.04f },
	{ NULL, 44, 0.04f },
	{ NULL, 45, 0.04f },
	{ NULL, 46, 0.04f },
	{ NULL, 47, 0.04f },
	{ NULL, 48, 0.04f },
	{ NULL, 49, 0.04f },
	{ NULL, 50, 0.04f },
	{ NULL, 51, 0.04f, true }
};

EntityFrame_t ShotcyclerAnimation_Fire [] =
{
	{ NULL, 1, 0.04f },
	{ NULL, 3, 0.04f },
	{ NULL, 5, 0.04f },
	{ NULL, 7, 0.04f },
	{ NULL, 9, 0.04f },
	{ NULL, 11, 0.04f },
	{ NULL, 13, 0.04f },
	{ NULL, 15, 0.04f },
	{ NULL, 17, 0.04f },
	{ Shotcycler_PrimaryAttack, 19, 0.04f, true }
};

EntityFrame_t ShotcyclerAnimation_Reload [] =
{
	{ NULL, 21, 0.01f },
	{ NULL, 22, 0.01f },
	{ NULL, 23, 0.01f },
	{ NULL, 24, 0.01f },
	{ NULL, 25, 0.01f },
	{ NULL, 26, 0.01f },
	{ NULL, 27, 0.01f },
	{ NULL, 28, 0.01f },
	{ NULL, 29, 0.01f },
	{ NULL, 30, 0.01f },
	{ NULL, 31, 0.01f },
	{ NULL, 32, 0.01f },
	{ NULL, 33, 0.01f },
	{ NULL, 34, 0.01f },
	{ NULL, 35, 0.01f },
	{ NULL, 36, 0.01f },
	{ NULL, 37, 0.01f },
	{ NULL, 29, 0.01f },
	{ NULL, 51, 0.01f },
	{ NULL, 52, 0.01f },
	{ NULL, 53, 0.01f },
	{ NULL, 54, 0.01f },
	{ NULL, 55, 0.01f },
	{ NULL, 56, 0.01f },
	{ NULL, 57, 0.01f },
	{ NULL, 58, 0.01f },
	{ NULL, 59, 0.01f },
	{ NULL, 60, 0.01f },
	{ NULL, 61, 0.01f },
	{ NULL, 62, 0.04f },
	{ NULL, 63, 0.04f },
	{ NULL, 64, 0.04f },
	{ NULL, 65, 0.04f },
	{ NULL, 66, 0.04f },
	{ NULL, 67, 0.04f },
	{ NULL, 68, 0.04f },
	{ NULL, 69, 0.04f },
	{ NULL, 70, 0.04f },
	{ NULL, 71, 0.04f },
	{ NULL, 72, 0.04f },
	{ NULL, 73, 0.04f },
	{ NULL, 74, 0.04f },
	{ NULL, 75, 0.04f },
	{ NULL, 76, 0.04f },
	{ NULL, 77, 0.04f },
	{ NULL, 78, 0.04f },
	{ NULL, 79, 0.04f },
	{ NULL, 80, 0.04f },
	{ NULL, 81, 0.04f },
	{ NULL, 82, 0.04f },
	{ NULL, 83, 0.04f },
	{ NULL, 84, 0.04f },
	{ NULL, 85, 0.04f },
	{ NULL, 86, 0.04f },
	{ NULL, 87, 0.04f },
	{ NULL, 88, 0.04f },
	{ NULL, 89, 0.04f },
	{ NULL, 90, 0.04f },
	{ NULL, 91, 0.04f },
	{ NULL, 92, 0.04f },
	{ NULL, 93, 0.04f },
	{ NULL, 94, 0.04f },
	{ NULL, 95, 0.04f },
	{ NULL, 96, 0.04f },
	{ NULL, 97, 0.04f },
	{ NULL, 98, 0.04f },
	{ NULL, 99, 0.04f },
	{ NULL, 100, 0.04f },
	{ NULL, 101, 0.04f },
	{ NULL, 102, 0.04f, true }
};

void Shotcycler_Precache(ServerEntity_t *ent)
{
	Server_PrecacheSound("weapons/shotcycler6/fire0.wav");
	Server_PrecacheSound("weapons/shotcycler6/fire1.wav");
}

void Shotcycler_Deploy(ServerEntity_t *ent)
{
	ent->local.iShotCycle = 6;

	// TODO: This should be on the client :(
	Sound(ent, CHAN_WEAPON, "weapons/shotcycler6/deploy.wav", 255, ATTN_NORM);

	Weapon_Animate(ent,ShotcyclerAnimation_Deploy);
}

void Shotcycler_Reload(ServerEntity_t *ent)
{
	ent->local.iShotCycle = 6;

	Weapon_Animate(ent,ShotcyclerAnimation_Reload);

	ent->local.dAttackFinished = Server.dTime+2;
}

void Shotcycler_PrimaryAttack(ServerEntity_t *ent)
{
	MathVector3f_t	vForward,vTemp;

	plAngleVectors(ent->v.v_angle,vForward,vTemp,vTemp);

	if(!ent->local.iShotCycle)
	{
		Shotcycler_Reload(ent);
		return;
	}
	else if(!ent->local.shotcycler_ammo)
	{
		// TODO: Use a generic global sound, anything specific just means more work, for such a minor detail.
		// TODO: This should also be on the client?
		Sound(ent,CHAN_WEAPON,"weapons/shotcycler6/shotcyclerclick.wav",255,ATTN_NORM);
		return;
	}

	Sound(ent, CHAN_WEAPON, va("weapons/shotcycler6/fire%i.wav", rand() % 2), 130, ATTN_NORM);

	Weapon_ViewPunch(ent, 5, false);
	ent->v.iPrimaryAmmo = ent->local.shotcycler_ammo -= 2;

	Entity_AddEffects(ent, EF_MUZZLEFLASH);

	ent->local.iShotCycle--;

	if(ent->local.attackb_finished > Server.dTime)	// No attack boost...
		ent->local.dAttackFinished = Server.dTime+ 2.5f;
	else
		ent->local.dAttackFinished = Server.dTime+ 5.0f;

	Weapon_BulletProjectile(ent,5.0f,5,vForward);

	Weapon_Animate(ent,ShotcyclerAnimation_Fire);
}
