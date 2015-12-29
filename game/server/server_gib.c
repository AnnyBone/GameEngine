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

void Gib_Spawn(MathVector3f_t mvOrigin, const char *cModel)
{
	int	j;
	ServerEntity_t *eGib;

	eGib = Entity_Spawn();
	if (eGib)
	{
		eGib->v.cClassname = "entity_gib";
		eGib->v.movetype = MOVETYPE_BOUNCE;
		eGib->v.TouchFunction = Gib_Touch;
		eGib->v.think = Entity_Remove;
		eGib->v.dNextThink = Server.dTime + 20.0f;

		Entity_SetPhysics(eGib, SOLID_TRIGGER, 0.5f, 3.0f);

		for (j = 0; j < 3; j++)
		{
		//	eGib->v.velocity[j] =
		//		eGib->v.avelocity[j] = (float)(rand() % 5 * eArea->v.iHealth * 5);
		}

		Entity_SetModel(eGib, (char*)cModel);
		Entity_SetOrigin(eGib, mvOrigin);
		Entity_SetSizeVector(eGib, g_mvOrigin3f, g_mvOrigin3f);
	}
}