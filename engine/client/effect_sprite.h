/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#pragma once

#ifdef __cplusplus

namespace core
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

		PLDraw *draw;

		bool
			islit,		// Is the sprite intended to be lit?
			isvisible,	// Is the sprite currently visible?
			isactive;	// Is the sprite considered active?

		MathVector3f_t
			position,
			mins, maxs;

		SpriteType_t type;

		PLColour colour;

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

extern core::SpriteManager *g_spritemanager;

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