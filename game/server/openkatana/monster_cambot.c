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

#include "server_monster.h"

/*
	SCAN BOT

	The Scan Bot, is a placeholder name, for the robots that fly around 
	Mishima’s fortress looking out for intruders. Of course it’s possible 
	these bots could also roam entire cities in the game world, looking out 
	for people that might be seeking to go against Mishima or otherwise 
	looking to break the law.

	The Scan Bot has no weapons but upon spotting anything hostile or 
	otherwise suspicious, it will immediately start an alarm and tell its 
	target to stay put.
*/

#define	SCANBOT_MODEL		"models/placeholders/sphere.md2"
#define	SCANBOT_HEALTH		40
#define	SCANBOT_MASS		0
#define	SCANBOT_FRICTION	1.0f

void ScanBot_Think(ServerEntity_t *entity);
void ScanBot_State(ServerEntity_t *entity);

void ScanBot_Spawn(ServerEntity_t *entity)
{
	Server_PrecacheModel(SCANBOT_MODEL);

	entity->v.netname		= "Scan Bot";
	entity->v.movetype		= MOVETYPE_WALK;
	entity->v.iHealth		= SCANBOT_HEALTH;
	entity->v.bTakeDamage	= true;

	Entity_SetPhysics(entity, SOLID_BBOX, SCANBOT_MASS, SCANBOT_FRICTION);
	Entity_SetModel(entity, SCANBOT_MODEL);
	Entity_SetSize(entity, -16, -16, -16, 16, 16, 16);

	// todo: redo this, legacy crap.
	entity->Monster.iType = MONSTER_SCANBOT;

	entity->ai.current_movement		= AI_MOVEMENT_FLYING;

	entity->ai.Think	= ScanBot_Think;
	entity->ai.State	= ScanBot_State;
}

void ScanBot_Think(ServerEntity_t *entity)
{
#if 0
	switch (entity->ai.current_think)
	{
	default:
	case MONSTER_THINK_IDLE:
		break;
	}
#else
	entity->ai.target_move = AI_GetVisibleMoveTarget(entity);
#endif
}

void ScanBot_State(ServerEntity_t *entity)
{
	switch (entity->ai.current_state)
	{
	default:
	case AI_STATE_ASLEEP:
		break;
	}
}