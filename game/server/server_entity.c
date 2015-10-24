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

#include "server_main.h"

/*
	General Entity Management
*/

/*	Create a new entity instance.
*/
ServerEntity_t *Entity_Spawn(void)
{
	ServerEntity_t *eSpawn = Engine.Spawn();

	// Set physics properties to their defaults!
	eSpawn->Physics.fMass		= 1.0f;
	eSpawn->Physics.fFriction	= 1.0f;
	eSpawn->Physics.fGravity	= SERVER_GRAVITY;

	return eSpawn;
}

/*	Alternative to SetOrigin.
	This is the only valid way to
	move an object without using
	the physics of the world
	(setting velocity and waiting).
	Directly changing origin will
	not set internal links correctly,
	so clipping would be messed up.
	This should be called when an
	object is spawned, and then only
	if it is teleported.
*/
void Entity_SetOrigin(ServerEntity_t *eEntity, MathVector3f_t vOrigin)
{
	Math_VectorCopy(vOrigin,eEntity->v.origin);

	Entity_Link(eEntity, false);
}

/*	Sets the angle of the given entity.
	Alternative to SetAngle.
*/
void Entity_SetAngles(ServerEntity_t *eEntity, MathVector3f_t vAngles)
{
	Math_VectorCopy(vAngles, eEntity->v.angles);

	// TODO: Link?
}

/*	Sets the model of the given entity.
	Alternative to SetModel.
*/
void Entity_SetModel(ServerEntity_t *eEntity,char *cModelPath)
{
	Engine.SetModel(eEntity,cModelPath);
}

/*	Sets the size of the given entity; requires that the model has been applied first.
	Alternative to SetSize.
*/
void Entity_SetSizeVector(ServerEntity_t *eEntity, MathVector3f_t vMin, MathVector3f_t vMax)
{
	int	i;

	// Check if the model is set yet, if not give us a little warning.
	if(!eEntity->v.model)
		Engine.Con_Warning("Setting entity size before model! (%s)\n",eEntity->v.cClassname);

	for(i = 0; i < 3; i++)
		if(vMin[i] > vMax[i])
		{
			Engine.Con_Warning("Backwards mins/maxs! (%s)\n",eEntity->v.cClassname);
			return;
		}

	Math_VectorCopy(vMin,eEntity->v.mins);
	Math_VectorCopy(vMax,eEntity->v.maxs);
	Math_VectorSubtract(vMax,vMin,eEntity->v.size);

	Entity_Link(eEntity, false);
}

/*	Set the size of the bounding box for the entity.
	Should be called AFTER setting model!
*/
void Entity_SetSize(ServerEntity_t *eEntity,
	float fMinA,float fMinB,float fMinC,
	float fMaxA,float fMaxB,float fMaxC)
{
	MathVector3f_t	vMin,vMax;

	vMin[0] = fMinA; vMin[1] = fMinB; vMin[2] = fMinC;
	vMax[0] = fMaxA; vMax[1] = fMaxB; vMax[2] = fMaxC;

	Entity_SetSizeVector(eEntity,vMin,vMax);
}

/*	Can be used for convenience.
*/
void Entity_AddEffects(ServerEntity_t *eEntity,int iEffects)
{
	if (eEntity->v.effects & iEffects)
		return;
	eEntity->v.effects |= iEffects;
}

/*	Can be used for convenience.
*/
void Entity_RemoveEffects(ServerEntity_t *eEntity,int iEffects)
{
	if (!(eEntity->v.effects & iEffects))
		return;
	eEntity->v.effects &= ~iEffects;
}

/*	Can be used for convenience.
*/
void Entity_ClearEffects(ServerEntity_t *eEntity)
{
	eEntity->v.effects = 0;
}

/*	Can be used for convenience.
*/
void Entity_AddFlags(ServerEntity_t *eEntity, int iFlags)
{
	if (eEntity->v.flags & iFlags)
		return;
	eEntity->v.flags |= iFlags;
}

/*	Can be used for convenience.
*/
void Entity_RemoveFlags(ServerEntity_t *eEntity, int iFlags)
{
	if (!(eEntity->v.flags & iFlags))
		return;
	eEntity->v.flags &= ~iFlags;
}

/*	Can be used for convenience.
*/
void Entity_ClearFlags(ServerEntity_t *eEntity, int iFlags)
{
	eEntity->v.flags = 0;
}

/*	Find a random spawn point for the entity (point_start).
	TODO: Rename!
*/
ServerEntity_t *Entity_SpawnPoint(ServerEntity_t *eEntity,int iType)
{
	ServerEntity_t	*eSpawnPoint;
	char	*cStartName;

	switch(iType)
	{
	case INFO_PLAYER_DEATHMATCH:
		cStartName = "point_deathmatch";
		break;
	case INFO_PLAYER_CTF:
		if(eEntity->local.pTeam == TEAM_RED)
			cStartName = "point_start_red";
		else if(eEntity->local.pTeam == TEAM_BLUE)
			cStartName = "point_start_blue";
		else
		{
			Engine.Con_Warning("Unknown team type for spawn point! (%i)",eEntity->local.pTeam);
			return NULL;
		}
		break;
#ifdef GAME_OPENKATANA
	case INFO_PLAYER_SUPERFLY:
	case INFO_PLAYER_MIKIKO:
#endif
	default:
		cStartName = "point_start";
	}

	eSpawnPoint = Engine.Server_FindEntity(Server.eWorld,cStartName,true);
	if(eSpawnPoint)
		if(iType == eSpawnPoint->local.style)
			return eSpawnPoint;

	return NULL;
}

/*	Simple function for checking if an entity can be damaged or not.
*/
bool Entity_CanDamage(ServerEntity_t *eEntity, ServerEntity_t *eTarget, int iDamageType)
{
	// Can't damage people on the same team.
	if(eEntity->local.pTeam && (eEntity->local.pTeam == eTarget->local.pTeam))
		return false;

	if(eTarget->v.bTakeDamage && (!eTarget->local.iDamageType || (eTarget->local.iDamageType == iDamageType)))
		return true;

	return false;
}

void Entity_Damage(ServerEntity_t *seEntity, ServerEntity_t *seInflictor, int iDamage, DamageType_t dtType)
{
	// Don't bother if there's no actual damage inflicted.
	if (iDamage <= 0)
		return;

	// Only continue if we can damage the entity.
	if (!Entity_CanDamage(seInflictor, seEntity, dtType))
		return;

	// If it's a monster or player, hand it over to the monster code.
	if (Entity_IsMonster(seEntity) || Entity_IsPlayer(seEntity))
	{
		Monster_Damage(seEntity, seInflictor, iDamage, dtType);
		return;
	}

	// Otherwise we'll do our own thing here...

	seEntity->v.iHealth -= iDamage;
	if (seEntity->v.iHealth <= 0)
	{
		if (seEntity->local.KilledFunction)
			seEntity->local.KilledFunction(seEntity, seInflictor);
		return;
	}

	if (seEntity->local.DamagedFunction)
		seEntity->local.DamagedFunction(seEntity, seInflictor);
}

/*	Damage entities within a specific radius.
*/
void Entity_RadiusDamage(ServerEntity_t *eInflictor, float fRadius, int iDamage, int iDamageType)
{
	int		i;
	float	fDistance;
	MathVector3f_t	vOrigin;
	ServerEntity_t *eTarget = Engine.Server_FindRadius(eInflictor->v.origin, fRadius);

	Engine.WriteByte(MSG_BROADCAST,SVC_TEMPENTITY);
	Engine.WriteByte(MSG_BROADCAST,CTE_EXPLOSION);

	for(i = 0; i < 3; i++)
		Engine.WriteCoord(MSG_BROADCAST,eInflictor->v.origin[i]);

	do
	{
		if(eTarget->v.bTakeDamage)
		{
			for(i = 0; i < 3; i++)
				vOrigin[i] = eTarget->v.origin[i]+(eTarget->v.mins[i]+eTarget->v.maxs[i])*0.5f;

			Math_VectorSubtract(eInflictor->v.origin,vOrigin,vOrigin);

			fDistance = 0.5f*(float)Math_VectorLength(vOrigin);
			if(fDistance > 0)
			{
				Math_VectorInverse(vOrigin);
				Math_VectorAdd(eTarget->v.velocity,vOrigin,eTarget->v.velocity);

				// Reduce the damage by distance.
				fDistance = (float)iDamage-(100.0f/fDistance);

				// Less damage for the inflictor.
				if(eTarget == eInflictor)
					fDistance = fDistance/2.0f;

				if(fDistance > 0)
					Entity_Damage(eTarget,eInflictor,(int)fDistance,iDamageType);
			}
		}

		eTarget = eTarget->v.chain;
	}
	while(eTarget);
}

/*	Link an entity.
*/
void Entity_Link(ServerEntity_t *eEntity,bool bTouchTriggers)
{
	Engine.LinkEntity(eEntity, bTouchTriggers);
}

void Entity_Unlink(ServerEntity_t *eEntity)
{
	Engine.UnlinkEntity(eEntity);
}

/*	Remove an entity from the world.
*/
void Entity_Remove(ServerEntity_t *eEntity)
{
	Engine.FreeEntity(eEntity);
}

/*
	Animation
*/

/*	Checks and updates the entity's current frame.
*/
void Entity_CheckFrames(ServerEntity_t *eEntity)
{
	// If something isn't active and Animationtime is over
	if(!eEntity->local.iAnimationEnd || (Server.dTime < eEntity->local.dAnimationTime))
		return;
	else if(eEntity->local.iAnimationCurrent > eEntity->local.iAnimationEnd)
	{
		eEntity->local.dAnimationTime = 0;
		return;
	}

	eEntity->v.frame = eEntity->local.iFrames[eEntity->local.iAnimationCurrent].iFrame;

	eEntity->local.dAnimationTime = Server.dTime+((double)eEntity->local.iFrames[eEntity->local.iAnimationCurrent].fSpeed);

	if(eEntity->local.iFrames[eEntity->local.iAnimationCurrent].Function)
		eEntity->local.iFrames[eEntity->local.iAnimationCurrent].Function(eEntity);

	eEntity->local.iAnimationCurrent++;
}

/*	Resets the currently active animation.
*/
void Entity_ResetAnimation(ServerEntity_t *eEntity)
{
	eEntity->v.frame					=
	eEntity->local.iAnimationCurrent	=
	eEntity->local.iAnimationEnd		= 0;

	// Reset the animation time.
	eEntity->local.dAnimationTime = 0;
}

/*	Start animating the entity.
*/
void Entity_Animate(ServerEntity_t *eEntity,EntityFrame_t *efAnimation)
{
	int i = 0;

	Entity_ResetAnimation(eEntity);

	eEntity->local.iAnimationCurrent = 0;

	eEntity->v.frame = efAnimation[0].iFrame;

	for(;;)
	{
		if(efAnimation[i].bIsEnd)
		{
			eEntity->local.iAnimationEnd = i;
			break;
		}

		i++;
	}

	eEntity->local.dAnimationTime	= Server.dTime+((double)efAnimation[1].fSpeed);
	eEntity->local.iFrames			= efAnimation;
}

/*
	Type Checks
*/

/*	Is the given monster a player?
*/
bool Entity_IsPlayer(ServerEntity_t *eEntity)
{
	if(eEntity->Monster.iType == MONSTER_PLAYER)
		return true;

	return false;
}

/*	Is the given monster a monster? (this is dumb...)
*/
bool Entity_IsMonster(ServerEntity_t *eEntity)
{
	if((eEntity->Monster.iType != MONSTER_PLAYER) && (eEntity->Monster.iType > MONSTER_VEHICLE))
		return true;

	return false;
}

/*
	Physics
*/

/*	Sets up the physical properties for the entity.
*/
void Entity_SetPhysics(ServerEntity_t *seEntity, PhysicsSolidTypes_t pstSolidType, float fMass, float fFriction)
{
	seEntity->Physics.iSolid = pstSolidType;
	seEntity->Physics.fMass = fMass;
	seEntity->Physics.fGravity = SERVER_GRAVITY;
	seEntity->Physics.fFriction = fFriction;
}

/*
	Math/Utility Functions
*/

void Entity_MakeVectors(ServerEntity_t *eEntity)
{
	Math_AngleVectors(eEntity->v.v_angle, eEntity->local.vForward, eEntity->local.vRight, eEntity->local.vUp);
}

bool Entity_DropToFloor(ServerEntity_t *eEntity)
{
	MathVector3f_t vEnd;
	trace_t	trGround;

	Math_VectorCopy(eEntity->v.origin, vEnd);

	vEnd[2] -= 256;

	trGround = Engine.Server_Move(eEntity->v.origin, eEntity->v.mins, eEntity->v.maxs, vEnd, false, eEntity);
	if (trGround.fraction == 1 || trGround.bAllSolid)
	{
		Engine.Con_Warning("Entity is stuck in world! (%s) (%i %i %i)\n", eEntity->v.cClassname,
			(int)eEntity->v.origin[0],
			(int)eEntity->v.origin[1],
			(int)eEntity->v.origin[2]);
		return false;
	}

	// Use SetOrigin so that it's automatically linked.
	Entity_SetOrigin(eEntity, trGround.endpos);

	Entity_AddFlags(eEntity, FL_ONGROUND);

	eEntity->v.groundentity = trGround.ent;

	return true;
}

bool Entity_IsTouching(ServerEntity_t *eEntity, ServerEntity_t *eOther)
{
	if (eEntity->v.mins[0] > eOther->v.maxs[0] ||
		eEntity->v.mins[1] > eOther->v.maxs[1] ||
		eEntity->v.mins[2] > eOther->v.maxs[2] ||
		eEntity->v.maxs[0] < eOther->v.mins[0] ||
		eEntity->v.maxs[1] < eOther->v.mins[1] ||
		eEntity->v.maxs[2] < eOther->v.mins[2])
		return false;

	return true;
}

/*
	IO System
*/
