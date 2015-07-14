/*	Copyright (C) 2011-2015 OldTimes Software
*/

extern "C" {
#include "server_main.h"
}

class CServerEntity
{
public:
	CServerEntity();
	~CServerEntity();

	void SetOrigin(MathVector3f_t mvOrigin);
	void SetAngles(MathVector3f_t mvAngles);
	void SetSize(MathVector3f_t mvMin, MathVector3f_t mvMax);
	void SetSize(float fMinA, float fMinB, float fMinC, float fMaxA, float fMaxB, float fMaxC);
	void SetModel(char *cPath);

	// Effects
	void AddEffects(int iEffects);
	void RemoveEffects(int iEffects);
	void ClearEffects();

	// Flags
	void AddFlags(int iFlags);
	void RemoveFlags(int iFlags);
	void ClearFlags();

	void Spawn();
	void Link(bool bTouchTriggers);
	void Unlink();
	void Free();
	
	/*	Returns the current entity reference for this particular entity.
	*/
	ServerEntity_t *GetEdict() 
	{
		return seEntity;
	};

	CServerEntity *seOwner;

private:
	ServerEntity_t *seEntity;
};

/*	Create a new entity instance.
*/
CServerEntity::CServerEntity()
{
	// Spawn the entity itself.
	Spawn();
}

CServerEntity::~CServerEntity()
{
	// Ensure that it's free before we delete ourself.
	Free();
}

void CServerEntity::Spawn()
{
	// Check to see if we've already spawned.
	if (seEntity)
		// If we have, free us, then let us spawn again.
		Free();

	// Spawn the entity.
	seEntity = Engine.Spawn();
}

void CServerEntity::Free()
{
	// Check to see if we've actually been spawned yet.
	if (!seEntity)
		// If not, then return.
		return;

	// Free us!
	Engine.FreeEntity(seEntity);
}

void CServerEntity::Link(bool bTouchTriggers)
{
	Engine.LinkEntity(seEntity, bTouchTriggers);
}

void CServerEntity::Unlink()
{
	Engine.UnlinkEntity(seEntity);
}

/*	Sets the size of the given entity; requires that the model has been applied first.
*/
void CServerEntity::SetSize(MathVector3f_t mvMin, MathVector3f_t mvMax)
{
	int i;

	// Ensure the model hasn't already been set.
	if (!seEntity->v.model)
		// Otherwise throw us a warning.
		Engine.Con_Warning("Setting entity size before model! (%s)\n", seEntity->v.cClassname);

	// Ensure the mins/maxs are the right way round.
	for (i = 0; i < 3; i++)
		if (mvMin[i] > mvMax[i])
		{
			Engine.Con_Warning("Backwards mins/maxs! (%s)\n", seEntity->v.cClassname);
			return;
		}

	Math_VectorCopy(mvMin, seEntity->v.mins);
	Math_VectorCopy(mvMax, seEntity->v.maxs);
	Math_VectorSubtract(mvMax, mvMin, seEntity->v.size);

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
	Math_VectorCopy(mvOrigin, seEntity->v.origin);

	Link(false);
}

/*	Sets the angle of the given entity.
*/
void CServerEntity::SetAngles(MathVector3f_t mvAngles)
{
	Math_VectorCopy(mvAngles, seEntity->v.angles);

	// TODO: Link?
}

/*	Sets the model of the given entity.
*/
void CServerEntity::SetModel(char *cPath)
{
	Engine.SetModel(seEntity, cPath);
}

/*
	Effects
	Utility functions for handling entity effects.
*/

void CServerEntity::AddEffects(int iEffects)
{
	seEntity->v.effects |= iEffects;
}

void CServerEntity::ClearEffects()
{
	seEntity->v.effects = 0;
}

void CServerEntity::RemoveEffects(int iEffects)
{
	seEntity->v.effects &= ~iEffects;
}

/*
	Flags
	Utility functions for handling entity flags.
*/

void CServerEntity::AddFlags(int iFlags)
{
	seEntity->v.flags |= iFlags;
}

void CServerEntity::ClearFlags()
{
	seEntity->v.flags = 0;
}

void CServerEntity::RemoveFlags(int iFlags)
{
	seEntity->v.flags &= ~iFlags;
}
