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

#ifndef SERVER_ENTITY_H
#define	SERVER_ENTITY_H

class ServerEntity
{
public:
	ServerEntity();
	~ServerEntity();

	void SetOrigin(MathVector3f_t origin);
	void SetAngles(MathVector3f_t angles);
	void SetSize(MathVector3f_t mins, MathVector3f_t maxs);
	void SetSize(float fMinA, float fMinB, float fMinC, float fMaxA, float fMaxB, float fMaxC);
	void SetModel(char *path);

	// Effects
	void AddEffects(int effects);
	void RemoveEffects(int effects);
	void ClearEffects();

	// Flags
	void AddFlags(int flags);
	void RemoveFlags(int flags);
	void ClearFlags();

	virtual void Spawn();
	virtual void Precache() {};

	bool CanDamage(ServerEntity *target, DamageType_t damagetype);
	void Damage(ServerEntity *inflictor, int damage, DamageType_t damagetype);
	virtual void Killed(ServerEntity *inflictor)	{};
	virtual void Damaged(ServerEntity *inflictor) {};

	// Physics
	bool IsTouching(ServerEntity *other);
	bool DropToFloor();

	void Link(bool touchtriggers);
	void Unlink();
	void Free();

	/*	Returns the current entity reference for this particular entity.
	*/
	ServerEntity_t *GetInstance()
	{
		return instance;
	};

	ServerEntity *owner;

private:
	ServerEntity_t *instance;
};

#endif	// SERVER_ENTITY_H