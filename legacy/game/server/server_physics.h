/*	Copyright (C) 2011-2014 OldTimes Software
*/
#ifndef __SERVERPHYSICS__
#define __SERVERPHYSICS__

#include "server_main.h"

void Physics_CheckVelocity(ServerEntity_t *eEntity);
void Physics_CheckWaterTransition(ServerEntity_t *eEntity);
void Physics_SetGravity(ServerEntity_t *eEntity);
void Physics_WallFriction(ServerEntity_t *eEntity, trace_t *trLine);
void Physics_Impact(ServerEntity_t *eEntity, ServerEntity_t *eOther);

bool Physics_CheckWater(ServerEntity_t *eEntity);

trace_t Physics_PushEntity(ServerEntity_t *eEntity, MathVector3f_t mvPush);

#endif
