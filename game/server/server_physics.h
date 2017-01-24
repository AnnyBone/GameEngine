//  Copyright (C) 2011-2017 OldTimes Software

#pragma once

#include "server_main.h"

PL_EXTERN_C

void Physics_CheckVelocity(ServerEntity_t *eEntity);
void Physics_CheckWaterTransition(ServerEntity_t *eEntity);
void Physics_SetGravity(ServerEntity_t *entity);
void Physics_WallFriction(ServerEntity_t *entity, trace_t *trace);
void Physics_Impact(ServerEntity_t *entity, ServerEntity_t *other);

bool Physics_CheckWater(ServerEntity_t *entity);

trace_t Physics_PushEntity(ServerEntity_t *entity, PLVector3D push);

PL_EXTERN_C_END
