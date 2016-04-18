#pragma once

#include "engine_base.h"

#include "shared_client.h"

int	Client_GetEffect(const char *cPath);
int Client_GetStat(ClientStat_t csStat);

void Client_PrecacheResource(int iType,char *cResource);

double Client_GetTime(void);

entity_t *Client_GetPlayerEntity(void);
entity_t *Client_GetViewEntity(void);

void Client_Simulate(void);