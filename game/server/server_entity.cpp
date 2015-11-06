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

/*	Create a new entity instance.
*/
CServerEntity::CServerEntity()
{
	// Reset everything...
	instance = 0;
}

CServerEntity::~CServerEntity()
{
	// Ensure that it's free before we delete ourself.
	Free();
}

void CServerEntity::Spawn()
{
	// Check to see if we've already spawned.
	if (instance)
		// If we have, free us, then let us spawn again.
		Free();

	// Spawn the entity.
	instance = Engine.Spawn();
}

void CServerEntity::Free()
{
	// Check to see if we've actually been spawned yet.
	if (!instance)
		// If not, then return.
		return;

	// Free us!
	Engine.FreeEntity(instance);
}

void CServerEntity::Link(bool touchtriggers)
{
	Engine.LinkEntity(instance, touchtriggers);
}

void CServerEntity::Unlink()
{
	Engine.UnlinkEntity(instance);
}

/*	Set the size of the bounding box for the entity.
	Should be called AFTER setting model!
*/
void CServerEntity::SetSize(MathVector3f_t mins, MathVector3f_t maxs)
{
	int i;

	// Ensure the model hasn't already been set.
	if (!instance->v.model)
		// Otherwise throw us a warning.
		Engine.Con_Warning("Setting entity size before model! (%s)\n", instance->v.cClassname);

	// Ensure the mins/maxs are the right way round.
	for (i = 0; i < 3; i++)
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
void CServerEntity::SetSize(
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
void CServerEntity::SetOrigin(MathVector3f_t origin)
{
	Math_VectorCopy(origin, instance->v.origin);

	Link(false);
}

/*	Sets the angle of the given entity.
*/
void CServerEntity::SetAngles(MathVector3f_t angles)
{
	Math_VectorCopy(angles, instance->v.angles);

	// TODO: Link?
}

/*	Sets the model of the given entity.
*/
void CServerEntity::SetModel(char *path)
{
	Engine.SetModel(instance, path);
}

/*
	Effects
	Utility functions for handling entity effects.
*/

void CServerEntity::AddEffects(int effects)
{
	instance->v.effects |= effects;
}

void CServerEntity::ClearEffects()
{
	instance->v.effects = 0;
}

void CServerEntity::RemoveEffects(int effects)
{
	instance->v.effects &= ~effects;
}

/*
	Flags
	Utility functions for handling entity flags.
*/

void CServerEntity::AddFlags(int flags)
{
	instance->v.flags |= flags;
}

void CServerEntity::ClearFlags()
{
	instance->v.flags = 0;
}

void CServerEntity::RemoveFlags(int flags)
{
	instance->v.flags &= ~flags;
}
