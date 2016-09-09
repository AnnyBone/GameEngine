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

void Gib_Touch(ServerEntity_t *seGib, ServerEntity_t *seOther)
{

}

void Gib_Bounce(ServerEntity_t *entity, ServerEntity_t *other)
{
	if (entity->v.flags & FL_ONGROUND)
		return;

	char sound[128];
	switch (entity->Physics.surface)
	{
	case MATERIAL_TYPE_GLASS:
		PHYSICS_SOUND_GLASS(sound);
		break;
	case MATERIAL_TYPE_WOOD:
		PHYSICS_SOUND_WOOD(sound);
		break;
	case MATERIAL_TYPE_CONCRETE:
		PHYSICS_SOUND_ROCK(sound);
		break;
	case MATERIAL_TYPE_METAL:
		PHYSICS_SOUND_METAL(sound);
		break;
	default:
		return;
	}

	Sound(entity, CHAN_AUTO, sound, 30, ATTN_NORM);
}

void Gib_Spawn(MathVector3f_t mvOrigin, const char *cModel)
{
	ServerEntity_t *gib = Entity_Spawn();
	if (gib)
	{
		gib->v.cClassname		= "entity_gib";
		gib->v.movetype			= MOVETYPE_BOUNCE;
		gib->v.TouchFunction	= Gib_Touch;
		gib->v.think			= Entity_Remove;
		gib->v.dNextThink		= Server.dTime + 20.0f;

		Entity_SetPhysics(gib, SOLID_TRIGGER, 0.5f, 3.0f);

		for (int j = 0; j < 3; j++)
		{
		//	eGib->v.velocity[j] =
		//		eGib->v.avelocity[j] = (float)(rand() % 5 * eArea->v.iHealth * 5);
		}

		Entity_SetModel(gib, (char*)cModel);
		Entity_SetOrigin(gib, mvOrigin);
		Entity_SetSizeVector(gib, pl_origin3f, pl_origin3f);

		// Get the physics property for this entity.
		gib->Physics.surface =
			seGetSkinPhysicsProperty(gib, seGetCurrentSkin(gib));
	}
}