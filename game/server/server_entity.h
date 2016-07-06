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

#pragma once

namespace game
{
	namespace server
	{
		class Entity
		{
		public:
			Entity();
			~Entity();

			void SetOrigin(plVector3f_t origin);
			void SetAngles(plVector3f_t angles);
			void SetSize(plVector3f_t mins, plVector3f_t maxs);
			void SetSize(float mina, float minb, float minc, float maxa, float maxb, float maxc);
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

			bool CanDamage(Entity *target, ServerDamageType_t damagetype);
			void Damage(Entity *inflictor, int damage, ServerDamageType_t damagetype);
			virtual void Killed(Entity *inflictor)	{};
			virtual void Damaged(Entity *inflictor) {};

			// Physics
			bool IsTouching(Entity *other);
			bool DropToFloor();

			void Link(bool touchtriggers);
			void Unlink();
			void Free();

			/*	Returns the current entity reference for this particular entity.
			*/
			ServerEntity_t *GetInstance()
			{
				return _instance;
			};

			Entity *owner;

		private:
			ServerEntity_t *_instance;
		};
	}
}