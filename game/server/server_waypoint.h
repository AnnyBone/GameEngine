/*	Copyright (C) 2011-2015 OldTimes Software

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

#pragma once

//#define	DEBUG_WAYPOINT

#define	MAX_WAYPOINTS	128

PL_EXTERN_C

void Waypoint_Initialize(void);
void Waypoint_Delete(Waypoint_t *point);
void Waypoint_Spawn(PLVector3D origin, WaypointType_t type);
void Waypoint_Frame();
void Waypoint_Draw(void);

Waypoint_t *Waypoint_GetByName(ServerEntity_t *entity, const char *name, float maxdistance);
Waypoint_t *Waypoint_GetByType(PLVector3D position, WaypointType_t type, float distance);
Waypoint_t *Waypoint_GetByVisibility(PLVector3D origin);

bool Waypoint_IsSafe(ServerEntity_t *eMonster, Waypoint_t *wPoint);

PL_EXTERN_C_END
