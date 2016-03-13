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
	{	MONSTER_LASERGAT,	MONSTER_LASERGAT,	RELATIONSHIP_LIKE				},
	{	MONSTER_LASERGAT,	MONSTER_INMATER,	RELATIONSHIP_LIKE				},
	{	MONSTER_LASERGAT,	MONSTER_PRISONER,	RELATIONSHIP_NEUTRAL			},
	{	MONSTER_LASERGAT,	MONSTER_COMPANION,	MONSTER_RELATIONSHIP_HATE		},
	{	MONSTER_LASERGAT,	MONSTER_PLAYER,		MONSTER_RELATIONSHIP_HATE		},

	// Inmater
	{	MONSTER_INMATER,	MONSTER_INMATER,	RELATIONSHIP_LIKE				},
	{	MONSTER_INMATER,	MONSTER_LASERGAT,	RELATIONSHIP_LIKE				},
	{	MONSTER_INMATER,	MONSTER_PRISONER,	RELATIONSHIP_NEUTRAL			},
	{	MONSTER_INMATER,	MONSTER_PLAYER,		MONSTER_RELATIONSHIP_HATE		},
	{	MONSTER_INMATER,	MONSTER_COMPANION,	MONSTER_RELATIONSHIP_HATE		},

	// Prisoner
	{	MONSTER_PRISONER,	MONSTER_PRISONER,	RELATIONSHIP_LIKE			},
	{	MONSTER_PRISONER,	MONSTER_PLAYER,		RELATIONSHIP_LIKE			},
	{	MONSTER_PRISONER,	MONSTER_COMPANION,	RELATIONSHIP_LIKE			},
	{	MONSTER_PRISONER,	MONSTER_LASERGAT,	MONSTER_RELATIONSHIP_HATE	},
	{	MONSTER_PRISONER,	MONSTER_INMATER,	MONSTER_RELATIONSHIP_HATE	},

	// Companion
	{	MONSTER_COMPANION,	MONSTER_COMPANION,	RELATIONSHIP_LIKE			},
	{	MONSTER_COMPANION,	MONSTER_PLAYER,		RELATIONSHIP_LIKE			},
	{	MONSTER_COMPANION,	MONSTER_PRISONER,	RELATIONSHIP_LIKE			},
	{	MONSTER_COMPANION,	MONSTER_LASERGAT,	MONSTER_RELATIONSHIP_HATE	},
	{	MONSTER_COMPANION,	MONSTER_INMATER,	MONSTER_RELATIONSHIP_HATE	},
#elif GAME_ADAMAS
	// Hurler
	{	MONSTER_HURLER,		MONSTER_PLAYER,		MONSTER_RELATIONSHIP_HATE	},
	{	MONSTER_HURLER,		MONSTER_HURLER,		RELATIONSHIP_LIKE			},
#endif
};

/**/

bool Monster_CheckBottom(ServerEntity_t *ent)
{
	MathVector3f_t	mins,maxs,start,stop;
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
}

bool Monster_MoveStep(ServerEntity_t *ent, plVector3f_t move, bool bRelink)
{
#if 0 // obsolete
	float			dz;
	plVector3f_t	vNewOrigin,end;
	trace_t			trace;
	int				i;

	Math_VectorClear(vNewOrigin);

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
	if(trace.bAllSolid)
		return false;
	else if(trace.bStartSolid)
	{
		vNewOrigin[2] -= MONSTER_STEPSIZE;

		trace = Engine.Server_Move(vNewOrigin,ent->v.mins,ent->v.maxs,end,false,ent);
		if(trace.bAllSolid || trace.bStartSolid)
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

void Monster_NewChaseDirection(ServerEntity_t *ent, MathVector3f_t target, float dist)
{
	float	deltax,deltay,d[3],tdir,olddir,turnaround;

	olddir = Math_AngleMod((ent->v.ideal_yaw/45.0f)*45.0f);
	turnaround = Math_AngleMod(olddir-180);

	deltax	= target[0]-ent->v.origin[0];
	deltay	= target[1]-ent->v.origin[1];
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

bool Monster_SetThink(ServerEntity_t *entity, MonsterThink_t newthink)
{
	if (entity->Monster.think == newthink)
		// Return false, then we might decide it's time for a different state.
		return false;
	else if (entity->Monster.state >= MONSTER_STATE_NONE)
	{
		Engine.Con_Warning("Attempted to set a think without a state! (%s)\n", entity->v.cClassname);
		return false;
	}

	entity->Monster.think = newthink;

	return true;
}

/*	Automatically sets the state for the monster.
*/
bool Monster_SetState(ServerEntity_t *eMonster, MonsterState_t msState)
{
	if (eMonster->Monster.state == msState)
		return true;

	switch(msState)
	{
	case MONSTER_STATE_AWAKE:
		if (eMonster->Monster.state == MONSTER_STATE_DEAD)
			return false;

		eMonster->Monster.state = MONSTER_STATE_AWAKE;
		break;
	case MONSTER_STATE_ASLEEP:
		if (eMonster->Monster.state == MONSTER_STATE_DEAD)
			return false;

		eMonster->Monster.state = MONSTER_STATE_ASLEEP;
		break;
	case MONSTER_STATE_DEAD:
		eMonster->Monster.state = MONSTER_STATE_DEAD;
		break;
	default:
		Engine.Con_Warning("Tried to set an unknown state for %s (%i)!\n",eMonster->v.cClassname,msState);
		return false;
	}

	return true;
}

/*	Called when a monster/entity gets killed.
*/
void Monster_Killed(ServerEntity_t *eTarget, ServerEntity_t *eAttacker, ServerDamageType_t type)
{
	if (eTarget->Monster.state == MONSTER_STATE_DEAD)
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
	else if(Entity_IsPlayer(eAttacker) && bIsMultiplayer)
	{
		char *cDeathMessage = "%s was killed by %s\n";

		if(eTarget == eAttacker)
		{
			cDeathMessage = "%s killed himself\n";

			eAttacker->v.iScore--;
		}
		else if(Entity_IsPlayer(eTarget) && bIsCooperative)
		{
			cDeathMessage = "%s was tk'd by %s (what a dick, huh?)";

			eAttacker->v.iScore--;
		}
		// Did we kill someone while dead?
		else
		{
			eAttacker->v.iScore++;

			// Extra points!
			if(eAttacker->v.iHealth <= 0)
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
					cDeathMessage = "%s was cooked pretty good by %s\n";
					break;
				case DAMAGE_TYPE_CRUSH:
					cDeathMessage = "%s was crushed by %s's fat ass\n";
					break;
				case DAMAGE_TYPE_EXPLODE:
					cDeathMessage = "%s was reduced to a bloody mess by %s\n";
					break;
				case DAMAGE_TYPE_FALL:
					cDeathMessage = "%s fell. Probably pushed by the cunning %s\n";
					break;
				case DAMAGE_TYPE_FREEZE:
					cDeathMessage = "%s is part of the decor, nice job %s!\n";
					break;
				case DAMAGE_TYPE_GRAVITY:
					cDeathMessage = "%s was killed by some gravity-induced weaponised thingy, by %s\n";
					break;
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
		eTarget->v.bTakeDamage = false;

	// Drop the currently equipped item for the player to pick up!
	Weapon_t *wActive = Weapon_GetCurrentWeapon(eTarget);
#ifdef GAME_OPENKATANA
	if(wActive && (wActive->iItem != WEAPON_LASERS))
#else
	if (wActive)
#endif
	{
		ServerEntity_t *eDroppedItem = Entity_Spawn();

		Math_VectorCopy(eTarget->v.origin,eDroppedItem->v.origin);

		eDroppedItem->local.style = wActive->iItem;

		Item_Spawn(eDroppedItem);
	}

	// Update our current state.
	eTarget->Monster.state = MONSTER_STATE_DEAD;
	if (eTarget->local.KilledFunction)
		eTarget->local.KilledFunction(eTarget, eAttacker, type);
}

void Monster_Damage(ServerEntity_t *target, ServerEntity_t *inflictor, int iDamage, ServerDamageType_t type)
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

	if (target->local.bBleed)
		ServerEffect_BloodPuff(target->v.origin);

	if (Entity_IsMonster(target))
	{
		// Automatically wake us up if asleep.
		if (target->Monster.state == MONSTER_STATE_ASLEEP)
			Monster_SetState(target, MONSTER_STATE_AWAKE);
	}

	// Only do this for players.
	if(Entity_IsPlayer(inflictor))
	{
#ifdef GAME_OPENKATANA
		if(inflictor->local.power_finished > Server.dTime)
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

	target->v.iHealth -= iDamage;

	if (target->local.DamagedFunction)
		target->local.DamagedFunction(target, inflictor, type);

	if ((target->Monster.state != MONSTER_STATE_DEAD) || (target->Monster.state != MONSTER_STATE_DYING))
		Monster_Killed(target, inflictor, type);
}

void MONSTER_WaterMove(ServerEntity_t *ent)
{
	if(ent->v.iHealth < 0 || ent->v.movetype == MOVETYPE_NOCLIP)
		return;

	if(ent->v.waterlevel != 3)
	{
		ent->local.dAirFinished = Server.dTime+12.0;
		ent->local.iDamage		= 2;
	}
	else if(ent->local.dAirFinished < Server.dTime && ent->local.dPainFinished < Server.dTime)
	{
		ent->local.iDamage += 2;
		if(ent->local.iDamage > 15)
			ent->local.iDamage = 10;

		Entity_Damage(ent, Server.eWorld, ent->local.iDamage, ent->local.iDamageType);

		ent->local.dPainFinished = Server.dTime+1.0;
	}

	if ((ent->v.watertype == BSP_CONTENTS_LAVA) && ent->local.dDamageTime < Server.dTime)
	{
		ent->local.dDamageTime = Server.dTime + 0.2;
	}
	else if ((ent->v.watertype == BSP_CONTENTS_SLIME) && ent->local.dDamageTime < Server.dTime)
	{
		ent->local.dDamageTime = Server.dTime + 1.0;
	}

	if(!(ent->v.flags & FL_WATERJUMP))
	{
		ent->v.velocity[0] = ent->v.velocity[1] = ent->v.velocity[2] =
			ent->v.velocity[2]-0.8f*ent->v.waterlevel*((float)Server.dTime)*(ent->v.velocity[0]+ent->v.velocity[1]+ent->v.velocity[2]);
	}
}

void Monster_MoveToGoal(ServerEntity_t *ent,MathVector3f_t goal,float distance)
{
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
}

/*	Returns the range from an entity to a target.
	TODO: Make this into a util type function.
*/
float MONSTER_GetRange(ServerEntity_t *ent, plVector3f_t target)
{
	plVector3f_t spot, spot1, spot2;

	spot1[0] = ent->v.origin[0]+ent->v.view_ofs[0];
	spot1[1] = ent->v.origin[1]+ent->v.view_ofs[1];
	spot1[2] = ent->v.origin[2]+ent->v.view_ofs[2];
	spot2[0] = target[0]; //+ target->v.view_ofs[0];
	spot2[1] = target[1]; //+ target->v.view_ofs[1];
	spot2[2] = target[2]; //+ target->v.view_ofs[2];

	Math_VectorSubtract(spot1,spot2,spot);

	return plLengthf(spot);
}

bool Monster_IsVisible(ServerEntity_t *ent,ServerEntity_t *target)
{
	trace_t	tTrace;
	MathVector3f_t vStart,vEnd;

	// TODO: Rework for new FOV system
	Math_VectorAdd(ent->v.origin,ent->v.view_ofs,vStart);
	Math_VectorAdd(target->v.origin,target->v.view_ofs,vEnd);

	tTrace = Traceline(ent,vStart,vEnd,true);
	if(!(tTrace.bOpen && tTrace.bWater) && tTrace.fraction == 1.0f)
		return true;

	return false;
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
			(eTargets != eMonster->local.eOwner)												&&	// Can't target owner.
			(eTargets != eMonster->Monster.eTarget && eTargets != eMonster->Monster.eOldTarget) &&	// Can't target an old target.
			(eTargets->Monster.iType != MONSTER_NONE))												// Has to be a monster.
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

	if (Monster_GetRelationship(Monster, Target) == MONSTER_RELATIONSHIP_HATE)
		return Target;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
//	NEW IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////

#define	MONSTER_EMOTION_RESET		30
#define	MONSTER_EMOTION_THRESHOLD	50

/*	Used to go over each monster state then update it, and then calls the monsters
	assigned think function.
*/
void Monster_Frame(ServerEntity_t *entity)
{
	// The following is only valid for actual monsters.
	if (!Entity_IsMonster(entity))
		return;

	Entity_CheckFrames(entity);

	// Handle jumping.
	if ((entity->local.jump_velocity < -300.0f) && (entity->v.flags & FL_ONGROUND))
	{
		entity->local.jump_velocity = 0;

		// Call up land function, so custom sounds can be added.
		if (entity->Monster.Land)
			entity->Monster.Land(entity);
	}
	else if (!(entity->v.flags & FL_ONGROUND))
		entity->local.jump_velocity = entity->v.velocity[2];

	if (entity->Monster.Frame)
		entity->Monster.Frame(entity);
}

/*
	Animation
*/

/*
	States
*/

/*
	Emotions
*/

/*	Check if it's time to reset the emotion state or not.
*/
bool Monster_EmotionReset(ServerEntity_t *eMonster, int iEmotion)
{
/*	if (eMonster->Monster.meEmotion[iEmotion].dResetDelay > Server.dTime)
	{
		eMonster->Monster.meEmotion[iEmotion].iEmotion = 0;

		eMonster->Monster.meEmotion[iEmotion].dResetDelay = Server.dTime + MONSTER_EMOTION_RESET;

#ifdef MONSTER_DEBUG
		Engine.Con_DPrintf("Reset emotional state for %s\n", eMonster->v.cClassname);
#endif

		return true;
	}*/

	return false;
}

/*
	Relationships
*/

/*	Checks our relationship against a table and returns how	we'll
	treat the current target.
*/
int	Monster_GetRelationship(ServerEntity_t *eMonster, ServerEntity_t *eTarget)
{
	int	i;

	if (!eMonster->Monster.iType)
	{
		Engine.Con_Warning("Attempted to get a relationship, but no monster type set! (%s)\n", eMonster->v.cClassname);
		return RELATIONSHIP_NEUTRAL;
	}

	// Run through the relationship table...
	for (i = 0; i < pARRAYELEMENTS(MonsterRelationship); i++)
	{
		// If the first type returns 0, then assume we've reached the end.
		if (!MonsterRelationship[i].iFirstType)
			break;

		if ((eMonster->Monster.iType == MonsterRelationship[i].iFirstType) &&
			(eTarget->Monster.iType == MonsterRelationship[i].iSecondType))
			return MonsterRelationship[i].iRelationship;
	}

	return RELATIONSHIP_NEUTRAL;
}

/*
	Movement
*/

/*	Can be used to debug monster movement / apply random movement.
*/
void Monster_MoveRandom(ServerEntity_t *eMonster,float fSpeed)
{
	// Add some random movement. ~hogsy
	if (rand() % 50 == 0)
	{
		int iResult = rand() % 3;
		if (iResult == 0)
			eMonster->v.velocity[0] += fSpeed;
		else if (iResult == 1)
			eMonster->v.velocity[0] -= fSpeed;

		iResult = rand() % 3;
		if (iResult == 0)
			eMonster->v.velocity[1] += fSpeed;
		else if (iResult == 1)
			eMonster->v.velocity[1] -= fSpeed;

		eMonster->v.angles[1] = plVectorToYaw(eMonster->v.velocity);
	}
	else if (rand() % 150 == 0)
		Monster_Jump(eMonster, 200.0f);
	else if (rand() % 250 == 0)
		eMonster->v.angles[1] = (float)(rand() % 360);
}

void Monster_MoveToWaypoint(ServerEntity_t *monster, Waypoint_t *target, float velocity)
{
	if (!target)
		return;
}

Waypoint_t *Monster_GetMoveTarget(ServerEntity_t *Monster)
{
	return Waypoint_GetByVisibility(Monster->v.origin);
}

/*	Move the monster forwards.
*/
void Monster_MoveForward(ServerEntity_t *monster, float velocity)
{
#if 0	// Ensure we're on the ground?
	if (!Entity_IsOnGround(monster))
		return;
#endif

	//MathVector3f_t angles;
}

/*	Move the monster backwards.
*/
void Monster_MoveBackward(ServerEntity_t *eMonster)
{}

/*	Strafe right.
*/
void Monster_MoveLeft(ServerEntity_t *Monster, float fVelocity)
{}

/*	Strafe left.
*/
void Monster_MoveRight(ServerEntity_t *eMonster)
{}

/*	Turn left on the spot.
*/
void Monster_TurnLeft(ServerEntity_t *Monster, float fVelocity)
{}

/*	Turn right on the spot.
*/
void Monster_TurnRight(ServerEntity_t *monster)
{}

/*	Allows a monster to jump with the given velocity.
*/
void Monster_Jump(ServerEntity_t *monster, float velocity)
{
	if (monster->v.velocity[2] != 0 || Entity_IsOnGround(monster))
		return;

	// Allow the monster to add additional sounds/movement if required.
	if (monster->Monster.Jump)
		monster->Monster.Jump(monster);

	monster->v.flags -= FL_ONGROUND;
	monster->v.velocity[2] = velocity;
}
