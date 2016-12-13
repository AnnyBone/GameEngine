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

#define	ROLLERBASE_MODEL	"models/editor/sphere.md2"

void Roller_Precache(ServerEntity_t *entity)
{
	Server_PrecacheModel(ROLLERBASE_MODEL);
}

void Roller_Spawn(ServerEntity_t *entity)
{
	Entity_SetModel(entity, ROLLERBASE_MODEL);
	Entity_SetOrigin(entity, entity->v.origin);
	Entity_SetSize(entity, -16.0f, -16.0f, -18.0f, 16.0f, 16.0f, 18.0f);
	Entity_SetPhysics(entity, SOLID_SLIDEBOX, 1.5f, 4.0f);

	entity->v.movetype = MOVETYPE_WALK;

	// Copy the current origin for later.
	MathVector3f_t oldorigin;
	Math_VectorCopy(entity->v.origin, oldorigin);

	// Try to drop it to the floor!
	if (!Entity_DropToFloor(entity))
		// Throw us back into the previous position.
		Entity_SetOrigin(entity, oldorigin);
}
