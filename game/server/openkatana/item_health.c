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

/*
	HIVEBOT HEALTH PACK
*/

#define	HEALTH_MODEL_BASE	"models/items/medkit1.md2"			//"models/items/health.md2"
#define	HEALTH_SOUND_PICKUP	"items/health/deploy.wav"

#define	HIVEBOT_MODEL_BASE	"models/placeholders/sphere.md2"	//"models/items/hivebot.md2"

#define	HEALTH_FLAG_OPEN	1

void Health_TouchFunction(ServerEntity_t *seHealth, ServerEntity_t *seOther);

void Health_Spawn(ServerEntity_t *seHealth)
{
	Server_PrecacheModel(HEALTH_MODEL_BASE);
	Server_PrecacheSound(HEALTH_SOUND_PICKUP);

	// Precaches for the Hive Bot...
	Server_PrecacheModel(HIVEBOT_MODEL_BASE);

	// Doesn't move.
	seHealth->v.movetype = MOVETYPE_NONE;

	seHealth->local.flags = 0;

	Entity_AddFlags(seHealth, FL_ITEM);

	Entity_SetTouchFunction(seHealth, Health_TouchFunction);
	Entity_SetPhysics(seHealth, SOLID_TRIGGER, 1.0f, 1.0f);
	Entity_SetModel(seHealth, HEALTH_MODEL_BASE);

	if (!Entity_DropToFloor(seHealth))
	{
		Engine.Con_Warning("Entity outside of world, removed! (%s)\n", seHealth->v.classname);
		Entity_Remove(seHealth);
		return;
	}
}

void Health_TouchFunction(ServerEntity_t *seHealth, ServerEntity_t *seOther)
{
	if (seHealth->local.flags & HEALTH_FLAG_OPEN)
		return;

	// TODO: Run animation of lil doors opening up.

	// TODO: Do this gradually?
	int i;
	for (i = 0; i < 5; i++)
	{
//		ServerEntity_t *seHiveBot;
	}

	// Set a flag so we don't come here again.
	seHealth->local.flags &= HEALTH_FLAG_OPEN;
}

/*
	HIVEBOT
*/

void HiveBot_Spawn(ServerEntity_t *seHiveBot)
{
	Entity_SetPhysics(seHiveBot, SOLID_TRIGGER, 0, 0);
	Entity_SetModel(seHiveBot, HIVEBOT_MODEL_BASE);
}