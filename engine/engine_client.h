#pragma once

#include "engine_base.h"

#include "shared_client.h"

PL_EXTERN_C

int	Client_GetEffect(const char *cPath);
int Client_GetStat(ClientStat_t csStat);

void Client_PrecacheResource(int type, const char *resource);

double Client_GetTime(void);

entity_t *Client_GetPlayerEntity(void);
entity_t *Client_GetViewEntity(void);

void Client_Simulate(void);

PL_EXTERN_C_END