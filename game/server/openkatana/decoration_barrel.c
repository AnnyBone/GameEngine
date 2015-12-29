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

#define	BARREL_MAX_HEALTH 45

typedef enum
{
	BARREL_STYLE_NORMAL,
	BARREL_STYLE_EXPLODE
} BarrelStyle_t;

void Barrel_Killed(ServerEntity_t *eBarrel, ServerEntity_t *seOther)
{
	if (eBarrel->local.style == BARREL_STYLE_EXPLODE)
	{
		Entity_RadiusDamage(eBarrel, MONSTER_RANGE_NEAR, 30, DAMAGE_TYPE_EXPLODE);

		if (Engine.Server_PointContents(eBarrel->v.origin) <= BSP_CONTENTS_WATER)
			Sound(eBarrel, CHAN_AUTO, SOUND_EXPLODE_UNDERWATER0, 255, ATTN_NORM);
		else
			Sound(eBarrel, CHAN_AUTO, SOUND_EXPLODE, 255, ATTN_NORM);
	}

	// TODO: gib

	// Update the model so it looks like it's actually exploded.
	Entity_SetModel(eBarrel, MODEL_DECORATION_BARREL3);

	// Don't take damage anymore.
	eBarrel->v.bTakeDamage = false;

	// Set it so it's no longer solid.
	eBarrel->Physics.iSolid = SOLID_NOT;
}

void Barrel_Damaged(ServerEntity_t *seBarrel, ServerEntity_t *seOther)
{
	// TODO: play sound, wobble, or react in some way...

	// Health has changed, update model.
	if (seBarrel->v.iHealth < 30)
	{
		if (seBarrel->v.iHealth < 10)
		{
			if (strcmp(seBarrel->v.model, MODEL_DECORATION_BARREL2))
			{
				// TODO: Spawn some gibs.

				Entity_SetModel(seBarrel, MODEL_DECORATION_BARREL2);
				Entity_SetSize(seBarrel, -10, -10, -15, 10, 9, 10);
				return;
			}
		}

		if (strcmp(seBarrel->v.model, MODEL_DECORATION_BARREL1))
		{
			// TODO: Spawn some gibs.

			Entity_SetModel(seBarrel, MODEL_DECORATION_BARREL1);
			Entity_SetSize(seBarrel, -10, -10, -15, 10, 9, 10);
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
	eBarrel->v.movetype = MOVETYPE_PUSH;
	eBarrel->v.bTakeDamage = true;

	eBarrel->local.iOldHealth = BARREL_MAX_HEALTH;
	eBarrel->local.cOldModel = MODEL_DECORATION_BARREL0;

	Entity_SetModel(eBarrel, MODEL_DECORATION_BARREL0);
	Entity_SetSize(eBarrel, -10, -10, -15, 10, 9, 10);
	Entity_SetPhysics(eBarrel, SOLID_SLIDEBOX, 2.0f, 5.0f);

	Entity_SetKilledFunction(eBarrel, Barrel_Killed);
	Entity_SetDamagedFunction(eBarrel, Barrel_Damaged);

	if (eBarrel->local.style == BARREL_STYLE_EXPLODE)
		// Switch the skin over.
		eBarrel->Model.iSkin = 1;

	// Link it up.
	Entity_DropToFloor(eBarrel);
}