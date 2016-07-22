/*
Copyright (C) 2011-2016 OldTimes Software

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

using namespace game::server;

// Create a new entity instance.
Entity::Entity() : _instance(nullptr)
{
}

Entity::~Entity()
{
	Free();
}

void Entity::Spawn()
{
	if (_instance)
		Free();

	// Set physics properties to their defaults!
	_instance->Physics.mass = 1.0f;
	_instance->Physics.friction = 1.0f;
	_instance->Physics.gravity = SERVER_GRAVITY;

	// Spawn the entity.
	_instance = g_engine->Spawn();
}

void Entity::Free()
{
	if (!_instance)
		return;

	g_engine->FreeEntity(_instance);
	_instance = 0;
}

void Entity::Link(bool touchtriggers)
{
	g_engine->LinkEntity(_instance, touchtriggers);
}

void Entity::Unlink()
{
	g_engine->UnlinkEntity(_instance);
}

/* Set the size of the bounding box for the entity.
 * Should be called AFTER setting model!
 */
void Entity::SetSize(plVector3f_t mins, plVector3f_t maxs)
{
	// Ensure the model hasn't already been set.
	if (!_instance->v.model)
		Engine.Con_Warning("Setting entity size before model! (%s)\n", _instance->v.cClassname);

	// Ensure the mins/maxs are the right way round.
	for (int i = 0; i < 3; i++)
		if (mins[i] > maxs[i])
		{
			Engine.Con_Warning("Backwards mins/maxs! (%s)\n", _instance->v.cClassname);
			return;
		}

	plVectorCopy(mins, _instance->v.mins);
	plVectorCopy(maxs, _instance->v.maxs);
	plVectorSubtract3fv(maxs, mins, _instance->v.size);

	Link(false);
}

/* Set the size of the bounding box for the entity.
 * Should be called AFTER setting model!
 */
void Entity::SetSize(
	float mina, float minb, float minc,
	float maxa, float maxb, float maxc)
{
	plVector3f_t mins, maxs;

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
void Entity::SetOrigin(plVector3f_t origin)
{
	plVectorCopy(origin, _instance->v.origin);

	Link(false);
}

/*	Sets the angle of the given entity.
*/
void Entity::SetAngles(MathVector3f_t angles)
{
	plVectorCopy(angles, _instance->v.angles);

	// TODO: Link?
}

/*	Sets the model of the given entity.
*/
void Entity::SetModel(char *path)
{
	g_engine->SetModel(_instance, path);
}

/*	Effects	*/

void Entity::AddEffects(int effects)
{
	_instance->v.effects |= effects;
}

void Entity::ClearEffects()
{
	_instance->v.effects = 0;
}

void Entity::RemoveEffects(int effects)
{
	_instance->v.effects &= ~effects;
}

/*	Flags	*/

void Entity::AddFlags(int flags)
{
	_instance->v.flags |= flags;
}

void Entity::ClearFlags()
{
	_instance->v.flags = 0;
}

void Entity::RemoveFlags(int flags)
{
	_instance->v.flags &= ~flags;
}

/*	Damage	*/

void Entity::Damage(Entity *inflictor, int damage, EntityDamageType_t damagetype)
{
	// Don't bother if there's no actual damage inflicted.
	if (damage <= 0)
		return;

	// Only continue if we can damage the entity.
	if (!CanDamage(inflictor, damagetype))
		return;

	_instance->v.iHealth -= damage;
	if (_instance->v.iHealth <= 0)
	{
		Killed(inflictor);
		return;
	}

	// TODO: Pass amount of damage?
	Damaged(inflictor);
}

bool Entity::CanDamage(Entity *target, EntityDamageType_t damagetype)
{
	if (!target->GetInstance()->v.bTakeDamage)
		return false;

	// Can't damage players on the same team.
	// Ensure we have an assigned team before checking this!
	if (_instance->local.pTeam && (_instance->local.pTeam == _instance->local.pTeam))
		return false;

	// See if we have a supported damage type.
	if	(!target->GetInstance()->local.iDamageType || (target->GetInstance()->local.iDamageType == damagetype))
		return true;

	// Can't damage.
	return false;
}

/*	Physics	*/

/*	Returns true if two entities are intersecting.
 */
bool Entity::IsTouching(Entity *other)
{
	if (_instance->v.mins[0] > other->GetInstance()->v.maxs[0] ||
		_instance->v.mins[1] > other->GetInstance()->v.maxs[1] ||
		_instance->v.mins[2] > other->GetInstance()->v.maxs[2] ||
		_instance->v.maxs[0] < other->GetInstance()->v.mins[0] ||
		_instance->v.maxs[1] < other->GetInstance()->v.mins[1] ||
		_instance->v.maxs[2] < other->GetInstance()->v.mins[2])
		return false;

	return true;
}

/*	Attempts to drop the entity to the floor.
 *	Returns true on success and false on failure.
 */
bool Entity::DropToFloor()
{
	plVector3f_t end;
	plVectorCopy(_instance->v.origin, end);
	end[2] -= 256;

	trace_t ground = Engine.Server_Move(_instance->v.origin, _instance->v.mins, _instance->v.maxs, end, false, _instance);
	if ((ground.fraction == 1) || ground.bAllSolid)
	{
		Engine.Con_Warning("Entity is stuck in world! (%s) (%i %i %i)\n",
			_instance->v.cClassname,
			(int)_instance->v.origin[0],
			(int)_instance->v.origin[1],
			(int)_instance->v.origin[2]);
		return false;
	}

	// Use SetOrigin so that it's automatically linked.
	SetOrigin(ground.endpos);

	AddFlags(FL_ONGROUND);

	_instance->v.groundentity = ground.ent;

	return true;
}