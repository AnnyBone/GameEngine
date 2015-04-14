/*	Copyright (C) 2011-2014 OldTimes Software
*/
#ifndef __SERVERPHYSICS__
#define __SERVERPHYSICS__

#include "server_main.h"

void Physics_CheckVelocity(edict_t *eEntity);
void Physics_CheckWaterTransition(edict_t *eEntity);
void Physics_SetGravity(edict_t *eEntity);
void Physics_WallFriction(edict_t *eEntity, trace_t *trLine);
void Physics_Impact(edict_t *eEntity, edict_t *eOther);

bool Physics_CheckWater(edict_t *eEntity);

trace_t Physics_PushEntity(edict_t *eEntity, MathVector3_t mvPush);

#endif
