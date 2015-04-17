/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "server_main.h"

void Barrel_Spawn(edict_t *eBarrel)
{
	Server_PrecacheModel(MODEL_BARREL);

	Entity_SetModel(eBarrel, MODEL_BARREL);
	Entity_SetSize(eBarrel, -16.0f, -16.0f, -24.0f, 16.0f, 16.0f, 40.0f);
}