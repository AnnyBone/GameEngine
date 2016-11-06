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

#include "server_main.h"

//#define	MONSTER_DEBUG

typedef struct
{
	int	iFirstType,iSecondType,	// The first and second monster types, which determines the encounter we might be dealing with.
		iRelationship;			// The relationship type between the two.
} MonsterRelationship_t;

#define MONSTER_NONE		0	// Used for entities that should not be treated as monsters.
#define MONSTER_PLAYER		1	// Clients / bots
#define	MONSTER_VEHICLE		2	// Vehicles
// Anything greater than 2 is assumed to be a monster by the engine!!!
#ifdef GAME_OPENKATANA
#define	MONSTER_PRISONER	3
#define	MONSTER_LASERGAT	4
#define	MONSTER_INMATER		5
#define MONSTER_SCANBOT		6

#define MONSTER_MIKIKO      50  // Added for sanity sake.
#define MONSTER_SUPERFLY    51  // Added for sanity sake.
#define	MONSTER_COMPANION	52	// Generic companion identifier.
#elif GAME_ADAMAS
#define MONSTER_HURLER	    3
#endif

#define	MONSTER_RANGE_MELEE		48.0f
#define MONSTER_RANGE_NEAR		256.0f
#define MONSTER_RANGE_MEDIUM	700.0f
#define MONSTER_RANGE_FAR		2000.0f

ServerEntity_t *Monster_GetTarget(ServerEntity_t *eMonster);
ServerEntity_t *Monster_GetEnemy(ServerEntity_t *Monster);

bool Monster_IsVisible(ServerEntity_t *entity, ServerEntity_t *target);

int	Monster_GetRelationship(ServerEntity_t *eMonster, ServerEntity_t *eTarget);

float MONSTER_GetRange(ServerEntity_t *ent, MathVector3f_t target);

void Monster_Damage(ServerEntity_t *target, ServerEntity_t *inflictor, int iDamage, EntityDamageType_t type);

void Monster_MoveToGoal(ServerEntity_t *ent, MathVector3f_t goal, float distance);
void AI_RandomMovement(ServerEntity_t *eMonster, float fSpeed);

void Monster_Killed(ServerEntity_t *monster, ServerEntity_t *attacker, EntityDamageType_t type);

Waypoint_t *AI_GetVisibleMoveTarget(ServerEntity_t *Monster);

/*
    Bots
*/

enum
{
	BOT_DEFAULT,
#ifdef GAME_OPENKATANA
	BOT_COMPANION
#endif
};
