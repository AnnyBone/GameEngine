/*	Copyright (C) 2011-2015 OldTimes Software
*/

#ifndef __ENTITY_H__
#define	__ENTITY_H__

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
		return Instance;
	};

	CServerEntity *seOwner;

private:
	ServerEntity_t *Instance;
};

#endif