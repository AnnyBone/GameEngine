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

#ifndef EFFECT_SPRITE_H
#define	EFFECT_SPRITE_H

typedef enum
{
	SPRITE_TYPE_DEFAULT,	// Depth-test, scaled manually and oriented.
	SPRITE_TYPE_SCALE,		// Depth-test, scale by view and oriented.
	SPRITE_TYPE_FLARE		// No depth-test, scale by view, always fullbright and oriented.
} SpriteType_t;

class Sprite
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

	bool IsVisible() { return isvisible; }
	bool IsActive()	{ return isactive; }
	bool IsLit() { return islit; }

	float GetScale() { return scale; }

	SpriteType_t GetType() { return Type; }

private:
	float scale;

	bool
		islit,
		isvisible,
		isactive;

	MathVector3f_t
		Position,
		mins, maxs;

	SpriteType_t Type;

	Colour_t Colour;

	Material_t *material;
};

class SpriteManager
{
public:
	SpriteManager();

	Sprite *Add();

	void Initialize();
	void Simulate();
	void Draw();
	void Shutdown();

private:
	std::vector<Sprite*> sprites;
};

extern SpriteManager *g_spritemanager;

#endif // !EFFECT_SPRITE_H