/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
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

			bool CanDamage(Entity *target, EntityDamageType_t damagetype);
			void Damage(Entity *inflictor, int damage, EntityDamageType_t damagetype);
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