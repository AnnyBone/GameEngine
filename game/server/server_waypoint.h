/*	Copyright (C) 2011-2014 OldTimes Software
*/
#ifndef __SERVERWAYPOINT_H__
#define __SERVERWAYPOINT_H__

//#define	DEBUG_WAYPOINT

#define	MAX_WAYPOINTS	128

void	Waypoint_Initialize(void);
void	Waypoint_Delete(Waypoint_t *point);
void	Waypoint_Spawn(vec3_t vOrigin,WaypointType_t type);
void	Waypoint_Frame(ServerEntity_t *eEntity);

Waypoint_t	*Waypoint_GetByName(ServerEntity_t *eMonster, char *cName, float fMaxDistance);
Waypoint_t	*Waypoint_GetByType(ServerEntity_t *eMonster, int iType, float fMaxDistance);
Waypoint_t	*Waypoint_GetByVisibility(vec3_t origin);

bool Waypoint_IsSafe(ServerEntity_t *eMonster, Waypoint_t *wPoint);

#endif
