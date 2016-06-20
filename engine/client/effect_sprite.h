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

		vlDraw_t *draw;

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
		~SpriteManager();

		Sprite *Add();
		void Remove(Sprite *sprite);
		void Clear();

		void Simulate();
		void Draw();

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