/*	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "server_main.h"

/*
	Used for both navigation and as basic reference points within levels.

	TODO:
		Save our waypoints to a map specific document.
*/

#define	WAYPOINT_MAX_ALLOCATED	2048

Waypoint_t	wWaypoints[WAYPOINT_MAX_ALLOCATED];

int	waypoint_count,	waypoint_allocated;

void Waypoint_Shutdown(void);

/*	Called per server spawn to reset waypoints and set everything up.
*/
void Waypoint_Initialize(void)
{
	memset(wWaypoints, 0, sizeof(wWaypoints));

	waypoint_count = 0;
}

bool Waypoint_IsSafe(ServerEntity_t *eMonster,Waypoint_t *wPoint)
{
#if 0
	ServerEntity_t *eMonsters;

	eMonsters = Engine.Server_FindRadius(wPoint->position,MONSTER_RANGE_NEAR);
	while(eMonsters)
	{
		// Check this by relationship rather than type.
		if(Monster_GetRelationship(eMonster,eMonsters) == MONSTER_RELATIONSHIP_HATE)
			return false;

		eMonsters = eMonsters->v.chain;
	}
#endif

	return true;
}

/*	Allocate a new waypoint so that we can use it.
	(Originally relied on dynamic allocation, but not anymore)
*/
Waypoint_t *Waypoint_Allocate(void)
{
	if ((waypoint_count + 1) > WAYPOINT_MAX_ALLOCATED)
	{
		Engine.Con_Warning("Failed to allocate waypoint! (%i / %i)\n", waypoint_count, WAYPOINT_MAX_ALLOCATED);
		return NULL;
	}

	return &wWaypoints[waypoint_count++];
}

void Waypoint_Delete(Waypoint_t *wPoint)
{
	if(!wPoint)
		return;

	free((void*)wPoint);

	waypoint_count--;
	waypoint_allocated--;
}

Waypoint_t *Waypoint_GetByVisibility(MathVector3f_t vOrigin)
{
	Waypoint_t	*wPoint;
	trace_t		tTrace;

	for (wPoint = wWaypoints; wPoint->number < waypoint_count; wPoint++)
	{
		tTrace = Traceline(NULL,vOrigin,wPoint->position,0);
		// Given point cannot be in the same place as the given origin.
		if(	Math_VectorCompare(tTrace.endpos,wPoint->position) && 
			!Math_VectorCompare(vOrigin,wPoint->position))
			return wPoint;
	}
	return NULL;
}

Waypoint_t *Waypoint_GetByNumber(int iWaypointNumber)
{
	Waypoint_t	*wPoint;

	for (wPoint = wWaypoints; wPoint->number < waypoint_count; wPoint++)
		if(wPoint->number == iWaypointNumber)
			return wPoint;

	return NULL;
}

Waypoint_t *Waypoint_GetByType(MathVector3f_t position, WaypointType_t type, float distance)
{
	Waypoint_t		*point;
	MathVector3f_t	vecdist;

	for (point = wWaypoints; point->number < waypoint_count; point++)
		if (point->wType == type)
		{
			Math_VectorSubtract(position, point->position, vecdist);
			if (Math_Length(vecdist) < distance)
				return point;
		}

	return NULL;
}

// [20/9/2012] Lets us find a specific waypoint by name ~hogsy
Waypoint_t *Waypoint_GetByName(ServerEntity_t *eMonster,char *cName,float fMaxDistance)
{
	Waypoint_t	*wPoint;
	vec3_t		vDistance;

	for (wPoint = wWaypoints; wPoint->number < waypoint_count; wPoint++)
		if(strcmp(wPoint->cName,cName))
		{
			// [20/9/2012] TODO: Needs testing :[ ~hogsy
			Math_VectorSubtract(eMonster->v.origin,wPoint->position,vDistance);
			if(Math_Length(vDistance) < fMaxDistance)
				return wPoint;
		}

	// [20/9/2012] Welp we didn't find anything, return null ~hogsy
	return NULL;
}

void Waypoint_Frame()
{
	if (waypoint_count <= 0)
		return;

	Waypoint_t *point;
	for (point = wWaypoints; point->number < waypoint_count; point++)
	{
	}
}

void Waypoint_Spawn(MathVector3f_t vOrigin,WaypointType_t type)
{
	int			iPointContents;
	Waypoint_t	*wPoint;

	/*	TODO
		If we're between two other waypoints
		and they can be seen then slot ourselves
		in so that we act as the last waypoint
		instead.
	*/

	iPointContents = Engine.Server_PointContents(vOrigin);
	// [17/6/2012] Check that this area is safe ~hogsy
	if(iPointContents == BSP_CONTENTS_SOLID)
	{
		Engine.Con_Warning("Failed to place waypoint, position is within a solid!\n");
		return;
	}

	{
		Waypoint_t *wVisibleWaypoint = Waypoint_GetByVisibility(vOrigin);
		// [30/1/2013] Oops! Check we actually have a visible waypoint!! ~hogsy
		if(wVisibleWaypoint)
		{
			MathVector3f_t vDistance;

			Math_VectorSubtract(wVisibleWaypoint->position,vOrigin,vDistance);
			if(Math_VectorLength(vDistance) < MONSTER_RANGE_MEDIUM)
			{
				Engine.Con_Printf("Invalid waypoint position!\n");
				return;
			}
		}
	}

	wPoint = Waypoint_Allocate();
	if(!wPoint)
	{
		Engine.Con_Warning("Failed to allocate waypoint!\n");
		return;
	}

	Math_VectorCopy(vOrigin,wPoint->position);

	wPoint->number	= waypoint_count;
	wPoint->bOpen	= false;
	wPoint->next	= Waypoint_GetByNumber(wPoint->number+1);
	wPoint->last	= Waypoint_GetByNumber(wPoint->number-1);
	wPoint->wType	= type;

	switch(type)
	{
	case WAYPOINT_ITEM:
		wPoint->cName = "item";
		break;
	case WAYPOINT_CLIMB:
		wPoint->cName = "climb";
		// TODO: Check that there's a ladder nearby.
		break;
	case WAYPOINT_COVER:
		wPoint->cName = "cover";
		// [27/12/2012] TODO: Check that this is actually cover ~hogsy
		break;
	case WAYPOINT_TYPE_JUMP:
		wPoint->cName = "jump";
		// [27/12/2012] TODO: Check if this is actually a jump by tracing out ahead ~hogsy
		break;
	case WAYPOINT_TYPE_SWIM:
		if(iPointContents != BSP_CONTENTS_WATER)
		{
			Engine.Con_Warning("Waypoint with type swim not within water contents (%i %i %i)!",
				(int)vOrigin[0],
				(int)vOrigin[1],
				(int)vOrigin[2]);

			Waypoint_Delete(wPoint);
			return;
		}

		wPoint->cName = "swim";
		break;
	case WAYPOINT_TYPE_DEFAULT:
		wPoint->cName = "default";
		break;
	case WAYPOINT_SPAWN:
		wPoint->cName = "spawn";
		break;
	default:
		Engine.Con_Warning("Unknown waypoint type (%i)!\n",type);

		Waypoint_Delete(wPoint);
		return;
	}

	// [30/1/2013] Pathetic reordering... Ugh ~hogsy
	if(!wPoint->last)
	{
		wPoint->last = Waypoint_GetByVisibility(vOrigin);
		if(!wPoint->last)
		{
			Engine.Con_Warning("Failed to get another visible waypoint! (%i)\n",wPoint->number);
			return;
		}
	}
	else if(wPoint->last != wPoint && wPoint->last->next)
		wPoint->last->next = wPoint;
}
