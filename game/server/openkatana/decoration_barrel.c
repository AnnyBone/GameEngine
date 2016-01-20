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

#include "server_main.h"

#include "server_effects.h"

#define	BARREL_MAX_HEALTH	30
#define	BARREL_MAX_DAMAGE	32

typedef enum
{
	BARREL_STYLE_NORMAL,
	BARREL_STYLE_EXPLODE
} BarrelStyle_t;

void Barrel_Think(ServerEntity_t *barrel)
{
	ServerEffect_Explosion(barrel->v.origin);

	Entity_RadiusDamage(barrel, MONSTER_RANGE_NEAR, BARREL_MAX_DAMAGE, DAMAGE_TYPE_EXPLODE);

	if (Engine.Server_PointContents(barrel->v.origin) <= BSP_CONTENTS_WATER)
		Sound(barrel, CHAN_AUTO, SOUND_EXPLODE_UNDERWATER0, 255, ATTN_NORM);
	else
		Sound(barrel, CHAN_AUTO, SOUND_EXPLODE, 255, ATTN_NORM);

	Entity_Remove(barrel);
}

void Barrel_Killed(ServerEntity_t *barrel, ServerEntity_t *seOther)
{
	// Don't take damage anymore.
	barrel->v.bTakeDamage = false;

	if (barrel->local.style == BARREL_STYLE_EXPLODE)
	{
		Entity_SetThinkFunction(barrel, Barrel_Think);
		barrel->v.dNextThink = Server.dTime + 0.275;
	}
	else
		Entity_SetModel(barrel, MODEL_DECORATION_BARREL3);
}

void Barrel_Damaged(ServerEntity_t *seBarrel, ServerEntity_t *seOther)
{
	char rockpath[MAX_QPATH];
	// TODO: play sound, wobble, or react in some way...

	if (seBarrel->v.iHealth < 10)
	{
		if (strcmp(seBarrel->v.model, MODEL_DECORATION_BARREL2))
		{
			PHYSICS_MODEL_ROCK(rockpath);

			ThrowGib(seBarrel->v.origin, g_mvOrigin3f, rockpath, 0, false);

			Entity_SetModel(seBarrel, MODEL_DECORATION_BARREL2);
			return;
		}
	}
	// Health has changed, update model.
	else if (seBarrel->v.iHealth < 25)
	{
		if (strcmp(seBarrel->v.model, MODEL_DECORATION_BARREL1))
		{
			PHYSICS_MODEL_ROCK(rockpath);

			ThrowGib(seBarrel->v.origin, g_mvOrigin3f, rockpath, 0, false);

			Entity_SetModel(seBarrel, MODEL_DECORATION_BARREL1);
			return;
		}
	}
}

void Barrel_Spawn(ServerEntity_t *eBarrel)
{
	Server_PrecacheModel(MODEL_DECORATION_BARREL0);
	Server_PrecacheModel(MODEL_DECORATION_BARREL1);
	Server_PrecacheModel(MODEL_DECORATION_BARREL2);
	Server_PrecacheModel(MODEL_DECORATION_BARREL3);

	eBarrel->v.iHealth = BARREL_MAX_HEALTH;
	eBarrel->v.movetype = MOVETYPE_TOSS;
	eBarrel->v.bTakeDamage = true;

	eBarrel->local.iOldHealth = BARREL_MAX_HEALTH;
	eBarrel->local.cOldModel = MODEL_DECORATION_BARREL0;

	Entity_SetModel(eBarrel, MODEL_DECORATION_BARREL0);
	Entity_SetSize(eBarrel, -16, -16, -15, 16, 16, 10);
	Entity_SetPhysics(eBarrel, SOLID_SLIDEBOX, 2.0f, 5.0f);

	Entity_SetKilledFunction(eBarrel, Barrel_Killed);
	Entity_SetDamagedFunction(eBarrel, Barrel_Damaged);

	if (eBarrel->local.style == BARREL_STYLE_EXPLODE)
		// Switch the skin over.
		eBarrel->Model.iSkin = 1;

	// Link it up.
	Entity_DropToFloor(eBarrel);
}