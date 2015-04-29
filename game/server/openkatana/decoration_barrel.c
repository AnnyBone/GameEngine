/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "server_main.h"

typedef enum
{
	BARREL_STYLE_NORMAL,
	BARREL_STYLE_EXPLODE
} BarrelStyle_t;

void Barrel_Killed(ServerEntity_t *eBarrel)
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

	Entity_Remove(eBarrel);
}

void Barrel_Damaged(ServerEntity_t *seBarrel)
{
	// TODO: play sound, wobble, or react in some way...
}

void Barrel_Spawn(ServerEntity_t *eBarrel)
{
	// Give us a random barrel model.
	char *cBarrelPath = MODEL_DECORATION_BARREL;

	Server_PrecacheModel(cBarrelPath);

	Entity_SetModel(eBarrel, cBarrelPath);
	Entity_SetSize(eBarrel, -10, -10, -15, 10, 9, 10);
	Entity_SetPhysics(eBarrel, SOLID_SLIDEBOX, 2.0f, 5.0f);

	Entity_SetKilledFunction(eBarrel, Barrel_Killed);
	Entity_SetDamagedFunction(eBarrel, Barrel_Damaged);
}