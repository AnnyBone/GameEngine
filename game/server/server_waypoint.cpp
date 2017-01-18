/*
Copyright (C) 2011-2017 Mark E Sowden <markelswo@gmail.com>

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
*/

#define	WAYPOINT_MAX_ALLOCATED	2048

namespace game
{
    namespace ai
    {
        ConsoleVariable_t waypoint_debug = {
                "waypoint_debug",
#if defined(_DEBUG)
                "1",
#else
                "0",
#endif
                false, true, "If enabled, waypoints will be drawn for host."
        };

        class WaypointManager
        {
        public:
            WaypointManager() {};
            ~WaypointManager() {};

            void Initialize();	// Initialization.
            void Simulate();	// Simulation of the waypoints.
            void Draw();		// Used for debugging.
            void Shutdown();	// Shutdown.

            Waypoint_t *Add();
            Waypoint_t *Get(std::string name);
            void Remove(Waypoint_t *waypoint);
            void Clear();

            float GetDistance(Waypoint_t *waypoint, PLVector3D position);

            bool IsClear(Waypoint_t *waypoint);
        protected:
        private:
            std::vector<Waypoint_t*> waypoints;
        };
    }

    extern ai::WaypointManager *waypoint_manager;
}

using namespace game;

ai::WaypointManager *waypoint_manager = nullptr;

void ai::WaypointManager::Initialize() {
    g_engine->Con_Printf("Initializing Waypoint Manager...\n");

    // Register console variables.
    g_engine->Cvar_RegisterVariable(&waypoint_debug, nullptr);

    // Base count is left pretty high, since we're
    // going to be dynamically tracking things.
    waypoints.reserve(WAYPOINT_MAX_ALLOCATED);
}

Waypoint_t *ai::WaypointManager::Add() {
    Waypoint_t *waypoint = new Waypoint_t;
    memset(waypoint, 0, sizeof(Waypoint_t));

    waypoints.push_back(waypoint);

    waypoint->number = (unsigned int)waypoints.size();

    Waypoint_t *last_waypoint = waypoints[waypoint->number - 1];
    if (last_waypoint) {
        waypoint->last = last_waypoint;
        last_waypoint->next = waypoint;
    }

    return waypoint;
}

Waypoint_t *ai::WaypointManager::Get(std::string name) {
    for (auto &point : waypoints) {
        if (strcmp(point->cName, name.c_str())) {
            return point;
        }
    }

    return nullptr;
}

void ai::WaypointManager::Remove(Waypoint_t *waypoint) {
    for (unsigned int i = 0; i < waypoints.size(); i++) {
        if (waypoints[i] == waypoint) {
            waypoints.erase(waypoints.begin() + i);
            delete waypoint;
            break;
        }
    }
}

void ai::WaypointManager::Clear() {
    waypoints.clear();                          // Clear the vector.
    waypoints.shrink_to_fit();                  // Clear out mem.
    waypoints.reserve(WAYPOINT_MAX_ALLOCATED);  // Reserve default amount, again.
}

float ai::WaypointManager::GetDistance(Waypoint_t *waypoint, PLVector3D position) {
    return position.Difference(waypoint->position);
}

void ai::WaypointManager::Simulate() {
//	for (auto &point : waypoints)
//	{
//	}
}

void ai::WaypointManager::Draw() {
    for (auto &point : waypoints) {
        if (point->next) {
            g_engine->DrawLine(point->position, point->next->position);
        }
        if (point->last) {
            g_engine->DrawLine(point->position, point->last->position);
        }
        core::draw::CoordinateAxes(point->position);
    }
}

void ai::WaypointManager::Shutdown() {
    Clear();
}

/*
	LEGACY MANAGEMENT SYSTEM
	DO NOT USE
*/

Waypoint_t	wWaypoints[WAYPOINT_MAX_ALLOCATED];

unsigned int waypoint_count;

void Waypoint_Shutdown(void);

// Called per server spawn to reset waypoints and set everything up.
void Waypoint_Initialize(void) {
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
Waypoint_t *Waypoint_Allocate(void) {
    if ((waypoint_count + 1) > WAYPOINT_MAX_ALLOCATED) {
        Engine.Con_Warning("Failed to allocate waypoint! (%i / %i)\n", waypoint_count, WAYPOINT_MAX_ALLOCATED);
        return NULL;
    }

    return &wWaypoints[waypoint_count++];
}

void Waypoint_Delete(Waypoint_t *wPoint) {
    if(!wPoint) {
        return;
    }

    memset(wPoint, 0, sizeof(Waypoint_t));

    waypoint_count--;
}

Waypoint_t *Waypoint_GetByVisibility(PLVector3D vOrigin) {
    trace_t	tTrace;
    for (Waypoint_t *wPoint = wWaypoints; wPoint->number < waypoint_count; wPoint++) {
        tTrace = Traceline(NULL,vOrigin,wPoint->position,0);
        // Given point cannot be in the same place as the given origin.
        if ((tTrace.endpos == wPoint->position) && !(vOrigin == wPoint->position))
            return wPoint;
    }
    return NULL;
}

Waypoint_t *Waypoint_GetByNumber(int iWaypointNumber) {
    for (Waypoint_t	*wPoint = wWaypoints; wPoint->number < waypoint_count; wPoint++) {
        if (wPoint->number == iWaypointNumber) {
            return wPoint;
        }
    }

    return NULL;
}

Waypoint_t *Waypoint_GetByType(PLVector3D position, WaypointType_t type, float distance)
{
    PLVector3D	vecdist;
    for (Waypoint_t	*point = wWaypoints; point->number < waypoint_count; point++) {
        if (point->wType == type) {
            vecdist = position - point->position;
            if (vecdist.Length() < distance) {
                return point;
            }
        }
    }

    return NULL;
}

Waypoint_t *Waypoint_GetByName(ServerEntity_t *entity, const char *name, float maxdistance) {
    for (Waypoint_t	*point = wWaypoints; point->number < waypoint_count; point++) {
        if (strcmp(point->cName, name)) {
            if (entity->v.origin.Difference(point->position) < maxdistance) {
                return point;
            }
        }
    }

    return NULL;
}

void Waypoint_Frame() {
    if (waypoint_count <= 0) {
        return;
    }

    for (Waypoint_t *point = wWaypoints; point->number < waypoint_count; point++) {
        if (point->next) {
            trace_t trace = g_engine->Server_Move(point->position, PLVector3D(0, 0, 0), PLVector3D(0, 0, 0),
                                                  point->next->position, 0, nullptr);
            if (!(point->next->position == trace.endpos)) {

            }
        }
    }
}

void Waypoint_Spawn(PLVector3D origin, WaypointType_t type) {
    /*	TODO
        If we're between two other waypoints
        and they can be seen then slot ourselves
        in so that we act as the last waypoint
        instead.
    */

    // Check that this area is safe.
    int contents = g_engine->Server_PointContents(origin);
    if(contents == BSP_CONTENTS_SOLID) {
        g_engine->Con_Warning("Failed to place waypoint, position is within a solid!\n");
        return;
    }

#if 0
    {
		Waypoint_t *wVisibleWaypoint = Waypoint_GetByVisibility(origin);
		// [30/1/2013] Oops! Check we actually have a visible waypoint!! ~hogsy
		if(wVisibleWaypoint)
		{
			MathVector3f_t vDistance;

			Math_VectorSubtract(wVisibleWaypoint->position,origin,vDistance);
			if(plVectorLength(vDistance) < MONSTER_RANGE_MEDIUM)
			{
				Engine.Con_Printf("Invalid waypoint position!\n");
				return;
			}
		}
	}
#endif

    Waypoint_t *point = Waypoint_Allocate();
    if(!point) {
        Engine.Con_Warning("Failed to allocate waypoint!\n");
        return;
    }

    point->position = origin;
    point->number   = waypoint_count;
    point->bOpen    = false;
    point->next	    = Waypoint_GetByNumber(point->number+1);
    point->last	    = Waypoint_GetByNumber(point->number-1);
    point->wType    = type;

    switch(type)
    {
        case WAYPOINT_ITEM:
            point->cName = "item";
            break;
        case WAYPOINT_CLIMB:
            point->cName = "climb";
            // TODO: Check that there's a ladder nearby.
            break;
        case WAYPOINT_COVER:
            point->cName = "cover";
            // [27/12/2012] TODO: Check that this is actually cover ~hogsy
            break;
        case WAYPOINT_TYPE_JUMP:
            point->cName = "jump";
            // [27/12/2012] TODO: Check if this is actually a jump by tracing out ahead ~hogsy
            break;
        case WAYPOINT_TYPE_SWIM:
            if(contents != BSP_CONTENTS_WATER)
            {
                Engine.Con_Warning("Waypoint with type swim not within water contents (%i %i %i)!",
                                   (int)origin.x,
                                   (int)origin.y,
                                   (int)origin.z);

                Waypoint_Delete(point);
                return;
            }

            point->cName = "swim";
            break;
        case WAYPOINT_TYPE_DEFAULT:
            point->cName = "default";
            break;
        case WAYPOINT_SPAWN:
            point->cName = "spawn";
            break;
        default:
            Engine.Con_Warning("Unknown waypoint type (%i)!\n",type);

            Waypoint_Delete(point);
            return;
    }

    if(!point->last) {
        point->last = Waypoint_GetByVisibility(origin);
        if(!point->last) {
            Engine.Con_Warning("Failed to get another visible waypoint! (%i)\n",point->number);
            return;
        }
    } else if(point->last != point && point->last->next) {
        point->last->next = point;
    }
}

void Waypoint_Draw(void) {
    for (Waypoint_t *point = wWaypoints; point->number < waypoint_count; point++) {
        if (point->next) {
            g_engine->DrawLine(point->position, point->next->position);
        }
        if (point->last) {
            g_engine->DrawLine(point->position, point->last->position);
        }
        g_engine->DrawCoordinateAxes(point->position);
    }
}
