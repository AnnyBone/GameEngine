#ifndef __KATCLIENT__
#define __KATCLIENT__

#include "EngineBase.h"

#include "shared_client.h"

int	Client_GetEffect(const char *cPath);
int Client_GetStat(ClientStat_t csStat);

void Client_PrecacheResource(int iType,char *cResource);

/*
	Utilities
*/

entity_t *Client_GetPlayerEntity(void);
entity_t *Client_GetViewEntity(void);

#endif