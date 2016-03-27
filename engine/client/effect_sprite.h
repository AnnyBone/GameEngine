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

#ifdef __cplusplus

namespace Core
{
	class Sprite : public ISprite
	{
	public:
		Sprite();
		virtual ~Sprite();

		void SetPosition(float x, float y, float z);
		void SetPosition(MathVector3f_t position);
		void SetColour(float r, float g, float b, float a = 1.0f);
		void SetType(SpriteType_t type);
		void SetActive(bool active) { isactive = active; }
		void SetScale(float scale);
		void SetMaterial(Material_t *material);

		virtual void Simulate();
		virtual void Draw();

		bool IsVisible()	{ return isvisible; }
		bool IsActive()		{ return isactive; }
		bool IsLit()		{ return islit; }

		float GetScale()		{ return scale; }
		int GetIdentifier()		{ return ident; }
		SpriteType_t GetType()	{ return type; }

	private:
		float scale;

		int	ident;

		bool
			islit,		// Is the sprite intended to be lit?
			isvisible,	// Is the sprite currently visible?
			isactive;	// Is the sprite considered active?

		MathVector3f_t
			position,
			mins, maxs;

		SpriteType_t type;

		plColour_t colour;

		Material_t *material;
	};

	class SpriteManager
	{
	public:
		SpriteManager();

		Sprite *Add();
		void Remove(Sprite *sprite);
		void Clear();

		void Initialize();
		void Simulate();
		void Draw();
		void Shutdown();

	private:
		std::vector<Sprite*> sprites;

		bool initialized;
	};
}

extern Core::SpriteManager *g_spritemanager;

#else

ISprite *SpriteManager_Add(void);
void SpriteManager_Remove(ISprite *sprite);
void SpriteManager_Clear(void);
void SpriteManager_Simulate(void);
void SpriteManager_Draw(void);

void Sprite_SetPosition(ISprite *sprite, MathVector3f_t position);
void Sprite_SetColour(ISprite *sprite, float r, float g, float b, float a);
void Sprite_SetType(ISprite *sprite, SpriteType_t type);
void Sprite_SetScale(ISprite *sprite, float scale);
void Sprite_SetMaterial(ISprite *sprite, Material_t *material);

#endif