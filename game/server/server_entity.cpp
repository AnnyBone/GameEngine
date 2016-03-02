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

/*	Create a new entity instance.
*/
ServerEntity::ServerEntity()
{
	// Reset everything...
	instance = 0;
}

ServerEntity::~ServerEntity()
{
	// Ensure that it's free before we delete ourself.
	Free();
}

void ServerEntity::Spawn()
{
	// Check to see if we've already spawned.
	if (instance)
		// If we have, free us, then let us spawn again.
		Free();

	// Set physics properties to their defaults!
	instance->Physics.fMass		= 1.0f;
	instance->Physics.fFriction = 1.0f;
	instance->Physics.fGravity	= SERVER_GRAVITY;

	// Spawn the entity.
	instance = Engine.Spawn();
}

void ServerEntity::Free()
{
	// Check to see if we've actually been spawned yet.
	if (!instance)
		// If not, then return.
		return;

	// Free us!
	Engine.FreeEntity(instance);
}

void ServerEntity::Link(bool touchtriggers)
{
	Engine.LinkEntity(instance, touchtriggers);
}

void ServerEntity::Unlink()
{
	Engine.UnlinkEntity(instance);
}

/*	Set the size of the bounding box for the entity.
	Should be called AFTER setting model!
*/
void ServerEntity::SetSize(MathVector3f_t mins, MathVector3f_t maxs)
{
	// Ensure the model hasn't already been set.
	if (!instance->v.model)
		// Otherwise throw us a warning.
		Engine.Con_Warning("Setting entity size before model! (%s)\n", instance->v.cClassname);

	// Ensure the mins/maxs are the right way round.
	for (int i = 0; i < 3; i++)
		if (mins[i] > maxs[i])
		{
			Engine.Con_Warning("Backwards mins/maxs! (%s)\n", instance->v.cClassname);
			return;
		}

	Math_VectorCopy(mins, instance->v.mins);
	Math_VectorCopy(maxs, instance->v.maxs);
	Math_VectorSubtract(maxs, mins, instance->v.size);

	Link(false);
}

/*	Set the size of the bounding box for the entity.
	Should be called AFTER setting model!
*/
void ServerEntity::SetSize(
	float mina, float minb, float minc,
	float maxa, float maxb, float maxc)
{
	MathVector3f_t mins, maxs;

	mins[0] = mina; mins[1] = minb; mins[2] = minc;
	maxs[0] = maxa; maxs[1] = maxb; maxs[2] = maxc;

	SetSize(mins, maxs);
}

/*	This is the only valid way to
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
void ServerEntity::SetOrigin(MathVector3f_t origin)
{
	Math_VectorCopy(origin, instance->v.origin);

	Link(false);
}

/*	Sets the angle of the given entity.
*/
void ServerEntity::SetAngles(MathVector3f_t angles)
{
	Math_VectorCopy(angles, instance->v.angles);

	// TODO: Link?
}

/*	Sets the model of the given entity.
*/
void ServerEntity::SetModel(char *path)
{
	Engine.SetModel(instance, path);
}

/*
	Effects
	Utility functions for handling entity effects.
*/

void ServerEntity::AddEffects(int effects)
{
	instance->v.effects |= effects;
}

void ServerEntity::ClearEffects()
{
	instance->v.effects = 0;
}

void ServerEntity::RemoveEffects(int effects)
{
	instance->v.effects &= ~effects;
}

/*
	Flags
	Utility functions for handling entity flags.
*/

void ServerEntity::AddFlags(int flags)
{
	instance->v.flags |= flags;
}

void ServerEntity::ClearFlags()
{
	instance->v.flags = 0;
}

void ServerEntity::RemoveFlags(int flags)
{
	instance->v.flags &= ~flags;
}

/*
	Damage
*/

void ServerEntity::Damage(ServerEntity *inflictor, int damage, ServerDamageType_t damagetype)
{
	// Don't bother if there's no actual damage inflicted.
	if (damage <= 0)
		return;

	// Only continue if we can damage the entity.
	if (!CanDamage(inflictor, damagetype))
		return;

	instance->v.iHealth -= damage;
	if (instance->v.iHealth <= 0)
	{
		Killed(inflictor);
		return;
	}

	// TODO: Pass amount of damage?
	Damaged(inflictor);
}

bool ServerEntity::CanDamage(ServerEntity *target, ServerDamageType_t damagetype)
{
	if (!target->GetInstance()->v.bTakeDamage)
		return false;

	// Can't damage players on the same team.
	// Ensure we have an assigned team before checking this!
	if (instance->local.pTeam && (instance->local.pTeam == instance->local.pTeam))
		return false;

	// See if we have a supported damage type.
	if	(!target->GetInstance()->local.iDamageType || (target->GetInstance()->local.iDamageType == damagetype))
		return true;

	// Can't damage.
	return false;
}

/*
	Physics
*/

bool ServerEntity::IsTouching(ServerEntity *other)
{
	if (instance->v.mins[0] > other->GetInstance()->v.maxs[0] ||
		instance->v.mins[1] > other->GetInstance()->v.maxs[1] ||
		instance->v.mins[2] > other->GetInstance()->v.maxs[2] ||
		instance->v.maxs[0] < other->GetInstance()->v.mins[0] ||
		instance->v.maxs[1] < other->GetInstance()->v.mins[1] ||
		instance->v.maxs[2] < other->GetInstance()->v.mins[2])
		return false;

	return true;
}

bool ServerEntity::DropToFloor()
{
	MathVector3f_t	end;
	trace_t			ground;

	Math_VectorCopy(instance->v.origin, end);

	end[2] -= 256;

	ground = Engine.Server_Move(instance->v.origin, instance->v.mins, instance->v.maxs, end, false, instance);
	if ((ground.fraction == 1) || ground.bAllSolid)
	{
		Engine.Con_Warning("Entity is stuck in world! (%s) (%i %i %i)\n",
			instance->v.cClassname,
			(int)instance->v.origin[0],
			(int)instance->v.origin[1],
			(int)instance->v.origin[2]);
		return false;
	}

	// Use SetOrigin so that it's automatically linked.
	SetOrigin(ground.endpos);

	AddFlags(FL_ONGROUND);

	instance->v.groundentity = ground.ent;

	return true;
}