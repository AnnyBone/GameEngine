/*	Copyright (C) 2011-2015 OldTimes Software
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
		Entity_SetSizeVector(eGib, mv3Origin, mv3Origin);
	}
}