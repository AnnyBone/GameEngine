/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "server_main.h"

/*	Create a new entity instance.
*/
CServerEntity::CServerEntity()
{
	// Reset everything...
}

CServerEntity::~CServerEntity()
{
	// Ensure that it's free before we delete ourself.
	Free();
}

void CServerEntity::Spawn()
{
	// Check to see if we've already spawned.
	if (Instance)
		// If we have, free us, then let us spawn again.
		Free();

	// Spawn the entity.
	Instance = Engine.Spawn();
}

void CServerEntity::Free()
{
	// Check to see if we've actually been spawned yet.
	if (!Instance)
		// If not, then return.
		return;

	// Free us!
	Engine.FreeEntity(Instance);
}

void CServerEntity::Link(bool bTouchTriggers)
{
	Engine.LinkEntity(Instance, bTouchTriggers);
}

void CServerEntity::Unlink()
{
	Engine.UnlinkEntity(Instance);
}

/*	Sets the size of the given entity; requires that the model has been applied first.
*/
void CServerEntity::SetSize(MathVector3f_t mvMin, MathVector3f_t mvMax)
{
	int i;

	// Ensure the model hasn't already been set.
	if (!Instance->v.model)
		// Otherwise throw us a warning.
		Engine.Con_Warning("Setting entity size before model! (%s)\n", Instance->v.cClassname);

	// Ensure the mins/maxs are the right way round.
	for (i = 0; i < 3; i++)
		if (mvMin[i] > mvMax[i])
		{
			Engine.Con_Warning("Backwards mins/maxs! (%s)\n", Instance->v.cClassname);
			return;
		}

	Math_VectorCopy(mvMin, Instance->v.mins);
	Math_VectorCopy(mvMax, Instance->v.maxs);
	Math_VectorSubtract(mvMax, mvMin, Instance->v.size);

	Link(false);
}

/*	Set the size of the bounding box for the entity.
	Should be called AFTER setting model!
*/
void CServerEntity::SetSize(
	float fMinA, float fMinB, float fMinC,
	float fMaxA, float fMaxB, float fMaxC)
{
	MathVector3f_t mvMin, mvMax;

	mvMin[0] = fMinA; mvMin[1] = fMinB; mvMin[2] = fMinC;
	mvMax[0] = fMaxA; mvMax[1] = fMaxB; mvMax[2] = fMaxC;

	SetSize(mvMin, mvMax);
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
void CServerEntity::SetOrigin(MathVector3f_t mvOrigin)
{
	Math_VectorCopy(mvOrigin, Instance->v.origin);

	Link(false);
}

/*	Sets the angle of the given entity.
*/
void CServerEntity::SetAngles(MathVector3f_t mvAngles)
{
	Math_VectorCopy(mvAngles, Instance->v.angles);

	// TODO: Link?
}

/*	Sets the model of the given entity.
*/
void CServerEntity::SetModel(char *cPath)
{
	Engine.SetModel(Instance, cPath);
}

/*
	Effects
	Utility functions for handling entity effects.
*/

void CServerEntity::AddEffects(int iEffects)
{
	Instance->v.effects |= iEffects;
}

void CServerEntity::ClearEffects()
{
	Instance->v.effects = 0;
}

void CServerEntity::RemoveEffects(int iEffects)
{
	Instance->v.effects &= ~iEffects;
}

/*
	Flags
	Utility functions for handling entity flags.
*/

void CServerEntity::AddFlags(int iFlags)
{
	Instance->v.flags |= iFlags;
}

void CServerEntity::ClearFlags()
{
	Instance->v.flags = 0;
}

void CServerEntity::RemoveFlags(int iFlags)
{
	Instance->v.flags &= ~iFlags;
}
