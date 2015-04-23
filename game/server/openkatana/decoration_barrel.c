/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "server_main.h"

void Barrel_Spawn(ServerEntity_t *eBarrel)
{
	Server_PrecacheModel(MODEL_DECORATION_BARREL0);

	Entity_SetModel(eBarrel, MODEL_DECORATION_BARREL0);
	Entity_SetSize(eBarrel, -10, -10, -15, 10, 9, 10);
}