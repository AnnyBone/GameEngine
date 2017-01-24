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
#include "server_effects.h"

// [20/5/2013] TODO: Move into game_resources.h ~hogsy
#define GLOCK_SOUND_NOAMMO	"weapons/glock/glockclick.wav"
#define GLOCK_SOUND_FIRE	"weapons/glock/glockfire.wav"
#define GLOCK_SOUND_RELOAD1	"weapons/glock/glockload.wav"
#define GLOCK_SOUND_RELOAD2	"weapons/glock/glockload2.wav";

void Glock_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,46,74,0.1f,0,0,0,FALSE);
}

void Glock_Reload(ServerEntity_t *ent)
{
	int		rs;
	char	*noise;

	if(ent->local.glock_ammo2 != 20)
	{
		// [20/5/2012] Fixed! ~hogsy
		rs = rand()%2;
		if (rs == 1)
			// [4/7/2012] TODO: Rename to glockload1.wav ~hogsy
			noise = GLOCK_SOUND_RELOAD1;
		else
			noise = GLOCK_SOUND_RELOAD2;

		Sound(ent,CHAN_WEAPON,noise,255,ATTN_NORM);

		//Weapon_Animate(ent,FALSE,30,74,0.05f,0,0,0,FALSE);

		ent->local.glock_ammo2		= 20;
		ent->local.dAttackFinished	= Server.time+2.6;
	}
}

// [4/7/2012] Renamed to Glock_PrimaryAttack ~hogsy
void Glock_PrimaryAttack(ServerEntity_t *ent)
{
	if(!ent->local.glock_ammo2)
	{
		Sound(ent,CHAN_WEAPON,GLOCK_SOUND_NOAMMO,255,ATTN_NORM);

		Glock_Reload(ent);

		return;
	}

	Sound(ent,CHAN_WEAPON,GLOCK_SOUND_FIRE,255,ATTN_NORM);

#if 0
	if(rand()%2 == 1)
		//Weapon_Animate(ent,FALSE,0,15,0.03f,10,19,0,FALSE);
	else
		//Weapon_Animate(ent,FALSE,16,27,0.03f,10,19,0,FALSE);
#endif

	ServerEffect_MuzzleFlash(ent->v.origin, ent->v.angles);

	ent->local.glock_ammo--;
	ent->v.primary_ammo	= ent->local.glock_ammo;

	Weapon_BulletProjectile(ent,2.0f,15,ent->local.forward);

	// [4/7/2012] Simplified ~hogsy
	ent->local.glock_ammo2--;

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+0.2;
	else
		ent->local.dAttackFinished = Server.time+0.35;
}
