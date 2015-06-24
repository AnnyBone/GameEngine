/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef	__ENGINESERVER__
#define	__ENGINESERVER__

#include "EngineBase.h"

#include "shared_server.h"

ServerEntity_t *Server_FindEntity(ServerEntity_t *eStartEntity,char *cName,bool bClassname);

void	Server_SinglePrint(ServerEntity_t *eEntity,char *cMessage);
void	Server_PrecacheResource(int iType,const char *ccResource);

double	Server_GetFrameTime(void);

#endif
