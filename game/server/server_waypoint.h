/*	Copyright (C) 2011-2014 OldTimes Software
*/
#ifndef __SERVERWAYPOINT_H__
#define __SERVERWAYPOINT_H__

#define	DEBUG_WAYPOINT

typedef enum
{
	WAYPOINT_DEFAULT,	// Basic point

	WAYPOINT_JUMP,		// Next waypoint needs a jump
	WAYPOINT_CLIMB,		// Next waypoint needs a climb
	WAYPOINT_COVER,		// Is behind cover
	WAYPOINT_ITEM,		// Has an item nearby
	WAYPOINT_WEAPON,	// Has a weapon nearby
	WAYPOINT_SPAWN,		// Near a spawn point
	WAYPOINT_SPAWNAREA,	// Near a spawn area
	WAYPOINT_SWIM		// Underwater
} WaypointType_t;

typedef struct waypoint_s
{
	const	char		*cName;		// The name for the waypoint
	int					number;		// Each point is assigned it's own number
	ServerEntity_t		*eEntity;	// The entity currently occupying that waypoint
#ifdef DEBUG_WAYPOINT
	ServerEntity_t		*eDebug;	// Entity to use for displaying waypoint position
#endif
	struct	waypoint_s	*next;		// The next point to target.
	struct	waypoint_s	*last;		// The last point we were at.
	vec3_t				position;	// The waypoints position.
	bool				bOpen;		// Check to see if the waypoint currently is occupied.
	WaypointType_t		wType;		// Type of point (duck, jump, climb etc.)
} Waypoint_t;

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
