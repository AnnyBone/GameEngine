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

#include "server_effects.h"
#include "server_player.h"
#include "server_item.h"
#include "server_weapon.h"

/*
	Base code for the AI. This contains various basic functions
	that handle animation, movement and other small features
	that are used for the various monsters that can be found
	in both OpenKatana and future projects.

	TODO:
		- Move out player specific code
		- Parse a document to find waypoints in the map
		- Add in tick stuff, if an entity is trying to
		run but hasn't moved from its last position and
		we're stuck then we'll get angry or if we're idle
		and doing nothing we might decide to wander around
		the environment a little.
		- Each monster should have its own list of "known"
		monsters which it has based sets for, if it doesn't
		know a monster such as the "player" and the player
		attacks it then we'll become an enemy.
		If we don't "know" an entity then our interest in
		it will increase, this will stimulate our wish to
		approach that entity. Depending on the response
		we'll update our memory table to reflect it so
		we can react on that.
		Also known as a relationship table fagget!
		- Implement enemy FOV system. Based on origin of
		the head position and then triangulated to four
		different points in-front of the creature which
		should be based on their maxview distance.
		Trace onto the world surface at the same time?
		 /a(a2)
		O - - -
		 \b(b2)

	This is our basic monster/ai platform which
	we'll be using for OpenKatana and other future
	projects which will be using the same code
	base...
	Worth noting that most of these will be shared
	between other entities, in other words if they're
	here then they must	function globally!
	Though that does not mean moving literally everything
	in here obviously.
	Entities must NOT make their own changes to any "tick"
	variables outside of here!
*/

#define MONSTER_STEPSIZE	18

// Relationships
MonsterRelationship_t MonsterRelationship[]=
{
#ifdef GAME_OPENKATANA
	// LaserGat
	{	MONSTER_LASERGAT,	MONSTER_LASERGAT,	AI_RELATIONSHIP_LIKE		},
	{	MONSTER_LASERGAT,	MONSTER_INMATER,	AI_RELATIONSHIP_LIKE		},
	{	MONSTER_LASERGAT,	MONSTER_PRISONER,	AI_RELATIONSHIP_NEUTRAL		},
	{	MONSTER_LASERGAT,	MONSTER_COMPANION,	AI_RELATIONSHIP_HATE		},
	{	MONSTER_LASERGAT,	MONSTER_PLAYER,		AI_RELATIONSHIP_HATE		},

	// Inmater
	{	MONSTER_INMATER,	MONSTER_INMATER,	AI_RELATIONSHIP_LIKE		},
	{	MONSTER_INMATER,	MONSTER_LASERGAT,	AI_RELATIONSHIP_LIKE		},
	{	MONSTER_INMATER,	MONSTER_PRISONER,	AI_RELATIONSHIP_NEUTRAL		},
	{	MONSTER_INMATER,	MONSTER_PLAYER,		AI_RELATIONSHIP_HATE		},
	{	MONSTER_INMATER,	MONSTER_COMPANION,	AI_RELATIONSHIP_HATE		},

	// Prisoner
	{	MONSTER_PRISONER,	MONSTER_PRISONER,	AI_RELATIONSHIP_LIKE	},
	{	MONSTER_PRISONER,	MONSTER_PLAYER,		AI_RELATIONSHIP_LIKE	},
	{	MONSTER_PRISONER,	MONSTER_COMPANION,	AI_RELATIONSHIP_LIKE	},
	{	MONSTER_PRISONER,	MONSTER_LASERGAT,	AI_RELATIONSHIP_HATE	},
	{	MONSTER_PRISONER,	MONSTER_INMATER,	AI_RELATIONSHIP_HATE	},

	// Companion
	{	MONSTER_COMPANION,	MONSTER_COMPANION,	AI_RELATIONSHIP_LIKE	},
	{	MONSTER_COMPANION,	MONSTER_PLAYER,		AI_RELATIONSHIP_LIKE	},
	{	MONSTER_COMPANION,	MONSTER_PRISONER,	AI_RELATIONSHIP_LIKE	},
	{	MONSTER_COMPANION,	MONSTER_LASERGAT,	AI_RELATIONSHIP_HATE	},
	{	MONSTER_COMPANION,	MONSTER_INMATER,	AI_RELATIONSHIP_HATE	},
#elif GAME_ADAMAS
	// Hurler
	{	MONSTER_HURLER,		MONSTER_PLAYER,		MONSTER_RELATIONSHIP_HATE	},
	{	MONSTER_HURLER,		MONSTER_HURLER,		RELATIONSHIP_LIKE			},
#else
	{ 0 },
#endif
};

/**/

bool Monster_CheckBottom(ServerEntity_t *ent)
{
#if 0
	PLVector3D	mins,maxs,start,stop;
	trace_t	trace;
	int		x,y;
	float	mid,bottom;

	Math_VectorAdd(ent->v.origin,ent->v.mins,mins);
	Math_VectorAdd(ent->v.origin,ent->v.maxs,maxs);

	/*	If all of the points under the corners are solid world, don't bother
		with the tougher checks
		the corners must be within 16 of the midpoint
	*/
	start[2] = mins[2]-1;
	for(x = 0; x <= 1; x++)
		for(y = 0; y <= 1; y++)
		{
			start[0] = x ? maxs[0] : mins[0];
			start[1] = y ? maxs[1] : mins[1];
			if(Engine.Server_PointContents(start) != BSP_CONTENTS_SOLID)
			{
				start[2] = mins[2];

				// The midpoint must be within 16 of the bottom
				start[0]	= stop[0]	= (mins[0]+maxs[0])*0.5f;
				start[1]	= stop[1]	= (mins[1]+maxs[1])*0.5f;

				stop[2] = start[2] - 2 * MONSTER_STEPSIZE;

				trace = Engine.Server_Move(start, pl_origin3f, pl_origin3f, stop, true, ent);
				if(trace.fraction == 1.0)
					return false;

				mid = bottom = trace.endpos[2];

				// The corners must be within 16 of the midpoint
				for(x = 0; x <= 1; x++)
					for(y = 0; y <= 1; y++)
					{
						start[0] = stop[0] = x ? maxs[0] : mins[0];
						start[1] = stop[1] = x ? maxs[1] : mins[1];

						trace = Engine.Server_Move(start, pl_origin3f, pl_origin3f, stop, true, ent);
						if(trace.fraction != 1.0 && trace.endpos[2] > bottom)
							bottom = trace.endpos[2];
						if(trace.fraction == 1.0 || mid-trace.endpos[2] > MONSTER_STEPSIZE)
							return false;
					}
			}
		}

	return true;
#else
    return false;
#endif
}

bool Monster_MoveStep(ServerEntity_t *ent, PLVector3D move, bool bRelink)
{
#if 0 // obsolete
	float			dz;
	plVector3f_t	vNewOrigin,end;
	trace_t			trace;
	int				i;

	plVectorClear(vNewOrigin);

	// Flying monsters don't step up
	if(ent->v.flags & (FL_SWIM|FL_FLY))
	{
		// Try one move with vertical motion, then one without
		for(i = 0; i < 2; i++)
		{
			Math_VectorAdd(ent->v.origin,move,vNewOrigin);

			if(i == 0)
			{
				dz = ent->v.origin[2] - ent->Monster.mvMoveTarget[2];
				if(dz > 40)
					vNewOrigin[2] -= 8;
				else if(dz < 30)
					vNewOrigin[2] += 8;
			}

			trace = Engine.Server_Move(ent->v.origin,ent->v.mins,ent->v.maxs,vNewOrigin,false,ent);
			if(trace.fraction == 1.0f)
			{
				if((ent->v.flags & FL_SWIM) && (Engine.Server_PointContents(trace.endpos) == BSP_CONTENTS_EMPTY))
					return false;

				Math_VectorCopy(trace.endpos,ent->v.origin);

				if(bRelink)
					Entity_Link(ent, true);

				return true;
			}
		}

		return false;
	}

	// Push down from a step height above the wished position
	vNewOrigin[2] += MONSTER_STEPSIZE;

	Math_VectorCopy(vNewOrigin,end);

	end[2] -= MONSTER_STEPSIZE*2;

	trace = Engine.Server_Move(vNewOrigin,ent->v.mins,ent->v.maxs,end,false,ent);
	if(trace.all_solid)
		return false;
	else if(trace.bStartSolid)
	{
		vNewOrigin[2] -= MONSTER_STEPSIZE;

		trace = Engine.Server_Move(vNewOrigin,ent->v.mins,ent->v.maxs,end,false,ent);
		if(trace.all_solid || trace.bStartSolid)
			return false;
	}

	if(trace.fraction == 1)
	{
		// If monster had the ground pulled out, go ahead and fall
		if(ent->v.flags & FL_PARTIALGROUND)
		{
			Math_VectorAdd(ent->v.origin,move,ent->v.origin);

			if(bRelink)
				Entity_Link(ent, true);

			ent->v.flags &= ~FL_ONGROUND;

			return true;
		}

		// Walked off an edge
		return false;
	}

	// Check point traces down for dangling corners
	Math_VectorCopy(trace.endpos,ent->v.origin);

	if(!Monster_CheckBottom(ent))
	{
		if(ent->v.flags & FL_PARTIALGROUND)
		{
			/*	Entity had floor mostly
				pulled out from underneath
				it and is trying to correct
			*/
			if(bRelink)
				Entity_Link(ent, true);

			return true;
		}

		return false;
	}

	if(ent->v.flags & FL_PARTIALGROUND)
		ent->v.flags &= ~FL_PARTIALGROUND;

	ent->v.groundentity = trace.ent;

	if(bRelink)
		Entity_Link(ent, true);
#endif
	return true;
}

bool Monster_StepDirection(ServerEntity_t *ent,float yaw,float dist)
{
#if 0
	plVector3f_t	move,oldorg;
	float			delta;

	ent->v.ideal_yaw	= yaw;
	ChangeYaw(ent);

	yaw	*= (float)pMath_PI*2/360;
	move[0] = (vec_t)cos(yaw)*dist;
	move[1] = (vec_t)sin(yaw)*dist;
	move[2] = 0;

	Math_VectorCopy(ent->v.origin,oldorg);
	if(Monster_MoveStep(ent,move,false))
	{
		delta = ent->v.angles[YAW]-ent->v.ideal_yaw;
		if(delta > 45 && delta < 315)
			Math_VectorCopy(oldorg,ent->v.origin);

		Entity_Link(ent, true);
		return true;
	}

	Entity_Link(ent, true);
#endif
	return false;
}

void Monster_NewChaseDirection(ServerEntity_t *ent, PLVector3D target, float dist)
{
	float	deltax,deltay,d[3],tdir,olddir,turnaround;

	olddir = Math_AngleMod((ent->v.ideal_yaw / 45.0f)*45.0f);
	turnaround = Math_AngleMod(olddir-180);

	deltax	= target.x-ent->v.origin.x;
	deltay	= target.y-ent->v.origin.y;
	if(deltax > 10)
		d[1] = 0;
	else if(deltax < -10)
		d[1] = 180;
	else
		d[1] = -1;

	if(deltay < -10)
		d[2] = 270;
	else if(deltay > 10)
		d[2] = 90;
	else
		d[2] = -1;

	// Try direct route
	if(d[1] != -1 && d[2] != -1)
	{
		if(!d[1])
			tdir = d[2] == 90.0f ? 45.0f:315.0f;
		else
			tdir = d[2] == 90.0f ? 135.0f:215.0f;

		if(tdir != turnaround && Monster_StepDirection(ent,tdir,dist))
			return;
	}

	// Try other directions
	if(((rand()&3)&1) || abs((int)deltay) > abs((int)deltax))
	{
		tdir	= d[1];
		d[1]	= d[2];
		d[2]	= tdir;
	}

	if(	d[1] != -1 && d[1] != turnaround	&&
		Monster_StepDirection(ent,d[1],dist))
		return;

	if(	d[2] != -1 && d[2] != turnaround	&&
		Monster_StepDirection(ent,d[2],dist))
		return;

	// There is no direct path to the player, so pick another direction
	if(olddir != -1 && Monster_StepDirection(ent,olddir,dist))
		return;

	// Randomly determine direction of search
	if(rand()&1)
	{
		for(tdir = 0; tdir <= 315; tdir += 45)
			if(tdir != turnaround && Monster_StepDirection(ent,tdir,dist))
				return;
	}
	else
	{
		for(tdir = 315; tdir >= 0; tdir -= 45)
			if(tdir!=turnaround && Monster_StepDirection(ent,tdir,dist))
				return;
	}

	if(turnaround != -1 && Monster_StepDirection(ent,turnaround,dist))
		return;

	ent->v.ideal_yaw = olddir;		// can't move

	// If a bridge was pulled out from underneath a monster, it may not have
	// a valid standing position at all
	if(!Monster_CheckBottom(ent))
		ent->v.flags |= FL_PARTIALGROUND;
}

/*	Called when a monster/entity gets killed.
*/
void Monster_Killed(ServerEntity_t *eTarget, ServerEntity_t *eAttacker, EntityDamageType_t type)
{
	if (eTarget->Monster.state == AI_STATE_DEAD)
		return;

	if(Entity_IsMonster(eTarget))
	{
		WriteByte(MSG_ALL, SVC_KILLEDMONSTER);

		Server.iMonsters--;
		eAttacker->v.iScore++;

#if 0
		// Update number of frags for client.
		Engine.SetMessageEntity(eAttacker);
		Engine.WriteByte(MSG_ONE,SVC_UPDATESTAT);
		Engine.WriteByte(MSG_ONE,STAT_FRAGS);
		Engine.WriteByte(MSG_ONE,eAttacker->v.iScore);
#endif
	}
	else if(Entity_IsPlayer(eAttacker) && g_ismultiplayer)
	{
		char *cDeathMessage = "%s was killed by %s\n";

		if(eTarget == eAttacker)
		{
			cDeathMessage = "%s killed himself\n";

			eAttacker->v.iScore--;
		}
		else if(Entity_IsPlayer(eTarget) && g_iscooperative)
		{
			cDeathMessage = "%s was tk'd by %s (what a dick, huh?)";

			eAttacker->v.iScore--;
		}
		// Did we kill someone while dead?
		else
		{
			eAttacker->v.iScore++;

			// Extra points!
			if(eAttacker->v.health <= 0)
			{
				// TODO: Play sound
				Engine.CenterPrint(eAttacker,"FROM BEYOND THE GRAVE!\n");

				cDeathMessage = "%s was killed from beyond the grave by %s\n";

				eAttacker->v.iScore += 2;
			}
			// Extra points!
			else if(!(eTarget->v.flags & FL_ONGROUND))
			{
				// TODO: Play sound
				Engine.CenterPrint(eAttacker,"WATCH THEM DROP!\n");

				cDeathMessage = "%s was shot out of the air by %s\n";

				eAttacker->v.iScore += 2;
			}
			else
			{
				switch (type)
				{
				case DAMAGE_TYPE_BURN:
					cDeathMessage = "%s was cooked pretty good by %s.\n";
					break;
				case DAMAGE_TYPE_CRUSH:
					cDeathMessage = "%s was crushed by %s's fat ass.\n";
					break;
				case DAMAGE_TYPE_EXPLODE:
					cDeathMessage = "%s was reduced to a bloody mess by %s.\n";
					break;
				case DAMAGE_TYPE_FALL:
					cDeathMessage = "%s fell. Probably pushed by the cunning %s.\n";
					break;
				case DAMAGE_TYPE_FREEZE:
					cDeathMessage = "%s is part of the decor, nice job %s!\n";
					break;
				case DAMAGE_TYPE_GRAVITY:
					cDeathMessage = "%s was killed by some gravity-induced weaponised thingy, by %s.\n";
					break;

				case DAMAGE_TYPE_DROWN: {
                    cDeathMessage = "%s drowned.\n";
                    break;
                }

                case DAMAGE_TYPE_NORMAL:
				case DAMAGE_TYPE_NONE:break;
				}
			}
		}

		// Update number of frags for client.
		Engine.SetMessageEntity(eAttacker);
		Engine.WriteByte(MSG_ONE, SVC_UPDATESTAT);
		Engine.WriteByte(MSG_ONE, STAT_FRAGS);
		Engine.WriteByte(MSG_ONE, eAttacker->v.iScore);

		// TODO: move Kill messages into mode_deathmatch (?) Create Weapon specific kill messages and more variations! ~eukos
		Engine.Server_BroadcastPrint(cDeathMessage, eTarget->v.netname, eAttacker->v.netname);
	}
	else
		eTarget->v.takedamage = false;

	// Drop the currently equipped item for the player to pick up!
	Weapon_t *wActive = Weapon_GetCurrentWeapon(eTarget);
#ifdef GAME_OPENKATANA
	if (wActive && (wActive->iItem != ITEM_WEAPON_LASERS))
#else
	if (wActive)
#endif
	{
		ServerEntity_t *eDroppedItem = Entity_Spawn();

        eDroppedItem->v.origin = eTarget->v.origin;

		eDroppedItem->local.style = wActive->iItem;

		Item_Spawn(eDroppedItem);
	}

	// Update our current state.
	eTarget->Monster.state = AI_STATE_DEAD;
	if (eTarget->local.KilledFunction)
		eTarget->local.KilledFunction(eTarget, eAttacker, type);
}

void Monster_Damage(ServerEntity_t *target, ServerEntity_t *inflictor, int iDamage, EntityDamageType_t type)
{
	/*	TODO
		If the new inflicted damage is greater
		than the last amount, we're a monster
		and	the attacker is client (or other)
		then we should change our target to this
		other entity as it's most likely more
		dangerous!
		Suggested that amount of damage should
		multiply on each call and then slowly
		decrease after sometime.
	*/

	if (target->local.bleed)
		ServerEffect_BloodPuff(target->v.origin);

	if (Entity_IsMonster(target))
	{
		// Automatically wake us up if asleep.
		if (target->Monster.state == AI_STATE_ASLEEP)
			AI_SetState(target, AI_STATE_AWAKE);
	}

	// Only do this for players.
	if(Entity_IsPlayer(inflictor))
	{
#ifdef GAME_OPENKATANA
		if(inflictor->local.power_finished > Server.time)
			iDamage *= 3;
#endif

		// Half the amount of damage we can inflict in hard.
		// Removed vita check here... Vita should not be acting like armor!
		if(cvServerSkill.value >= 3)
			iDamage /= 2;
	}
	else if(Entity_IsMonster(inflictor))
	{
		// Double if we're a monster.
		if(cvServerSkill.value >= 3)
			iDamage *= 2;
	}

	target->v.health -= iDamage;

	if (target->local.DamagedFunction)
		target->local.DamagedFunction(target, inflictor, type);

	if ((target->v.health <= 0) && ((target->Monster.state != AI_STATE_DEAD) || (target->Monster.state != AI_STATE_DYING)))
		Monster_Killed(target, inflictor, type);
}

void MONSTER_WaterMove(ServerEntity_t *ent)
{
	if(ent->v.health < 0 || ent->v.movetype == MOVETYPE_NOCLIP)
		return;

	if(ent->v.waterlevel != 3)
	{
		ent->local.air_finished = Server.time+12.0;
		ent->local.damage		= 2;
	}
	else if(ent->local.air_finished < Server.time && ent->local.pain_finished < Server.time)
	{
		ent->local.damage += 2;
		if(ent->local.damage > 15)
			ent->local.damage = 10;

		Entity_Damage(ent, Server.world, ent->local.damage, DAMAGE_TYPE_DROWN);

		ent->local.pain_finished = Server.time+1.0;
	}

	if ((ent->v.watertype == BSP_CONTENTS_LAVA) && ent->local.dDamageTime < Server.time)
	{
		ent->local.dDamageTime = Server.time + 0.2;
	}
	else if ((ent->v.watertype == BSP_CONTENTS_SLIME) && ent->local.dDamageTime < Server.time)
	{
		ent->local.dDamageTime = Server.time + 1.0;
	}

	if(!(ent->v.flags & FL_WATERJUMP))
	{
		ent->v.velocity.x = ent->v.velocity.y = ent->v.velocity.z =
			ent->v.velocity.z-0.8f*ent->v.waterlevel*((float)Server.time)*(ent->v.velocity.x+ent->v.velocity.y+ent->v.velocity.z);
	}
}

void Monster_MoveToGoal(ServerEntity_t *ent,PLVector3D goal,float distance)
{
#if 0
	int i;

	if(!(ent->v.flags & (FL_ONGROUND|FL_FLY|FL_SWIM)))
		return;

	for(i = 0; i < 3; i++)
	{
		if(goal[i] > ent->v.absmax[i]+distance)
			return;
		if(goal[i] < ent->v.absmin[i]+distance)
			return;
	}

	if((rand()&3) == 1 || !Monster_StepDirection(ent,ent->v.ideal_yaw,distance))
		Monster_NewChaseDirection(ent,goal,distance);	// Change to goal at some point
#endif
}

/*	Returns the range from an entity to a target.
	TODO: Make this into a util type function.
*/
float MONSTER_GetRange(ServerEntity_t *ent, PLVector3D target)
{
#if 0
	PLVector3D spot, spot1, spot2;

	spot1[0] = ent->v.origin[0]+ent->v.view_ofs[0];
	spot1[1] = ent->v.origin[1]+ent->v.view_ofs[1];
	spot1[2] = ent->v.origin[2]+ent->v.view_ofs[2];
	spot2[0] = target[0]; //+ target->v.view_ofs[0];
	spot2[1] = target[1]; //+ target->v.view_ofs[1];
	spot2[2] = target[2]; //+ target->v.view_ofs[2];

	Math_VectorSubtract(spot1,spot2,spot);

	return plLengthf(spot);
#else
    return 0;
#endif
}

bool Monster_IsVisible(ServerEntity_t *ent,ServerEntity_t *target)
{
#if 0
	trace_t	tTrace;
	PLVector3D vStart,vEnd;

	// TODO: Rework for new FOV system
	Math_VectorAdd(ent->v.origin,ent->v.view_ofs,vStart);
	Math_VectorAdd(target->v.origin,target->v.view_ofs,vEnd);

	tTrace = Traceline(ent,vStart,vEnd,true);
	if(!(tTrace.bOpen && tTrace.bWater) && tTrace.fraction == 1.0f)
		return true;

	return false;
#else
    return false;
#endif
}

/*	Returns view target.
	TODO:
		- Base this on if we're currently facing a target or not.
*/
ServerEntity_t *Monster_GetTarget(ServerEntity_t *eMonster)
{
	ServerEntity_t	*eTargets = Engine.Server_FindRadius(eMonster->v.origin,10000.0f);	//eMonster->Monster.fViewDistance);

	do
	{
		// Only return if it's a new target and a monster type!
		if(	(eMonster != eTargets)																&&	// Can't target ourself.
			(eTargets != eMonster->local.owner)												&&	// Can't target owner.
			(eTargets != eMonster->Monster.eTarget && eTargets != eMonster->Monster.eOldTarget) &&	// Can't target an old target.
			(eTargets->Monster.type != MONSTER_NONE))												// Has to be a monster.
			// Quick crap thrown in to check if the target is visible or not...
			if(Monster_IsVisible(eMonster,eTargets))
				return eTargets;

		eTargets = eTargets->v.chain;
	} while(eTargets);

	return NULL;
}

ServerEntity_t *Monster_GetEnemy(ServerEntity_t *Monster)
{
	ServerEntity_t *Target = Monster_GetTarget(Monster);
	if (!Target)
		return NULL;

	if (Monster_GetRelationship(Monster, Target) == AI_RELATIONSHIP_HATE)
		return Target;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
//	NEW IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////



/*
	Targetting
*/

Waypoint_t *AI_GetVisibleMoveTarget(ServerEntity_t *entity)
{
	Waypoint_t *new_waypoint = Waypoint_GetByVisibility(entity->v.origin);
	if (entity->ai.target_move && (new_waypoint == entity->ai.target_move))
	{
		// Try to get a different waypoint.
		if (new_waypoint->next)			return new_waypoint->next;
		else if (new_waypoint->last)	return new_waypoint->last;
	}

	return new_waypoint;
}

/*
	Relationships
*/

/*	Checks our relationship against a table and returns how	we'll
	treat the current target.
*/
int	Monster_GetRelationship(ServerEntity_t *entity, ServerEntity_t *target)
{
	int	i;

	if (!entity->Monster.type)
		return AI_RELATIONSHIP_NEUTRAL;

	// Run through the relationship table...
	for (i = 0; i < plArrayElements(MonsterRelationship); i++)
	{
		// If the first type returns 0, then assume we've reached the end.
		if (!MonsterRelationship[i].iFirstType)
			break;

		if ((entity->Monster.type == MonsterRelationship[i].iFirstType) &&
			(target->Monster.type == MonsterRelationship[i].iSecondType))
			return MonsterRelationship[i].iRelationship;
	}

	return AI_RELATIONSHIP_NEUTRAL;
}