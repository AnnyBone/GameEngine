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

/*
	Sprites
*/

typedef enum
{
	SPRITE_TYPE_DEFAULT,	// Depth-test, scaled manually and oriented.
	SPRITE_TYPE_SCALE,		// Depth-test, scale by view and oriented.
	SPRITE_TYPE_FLARE		// No depth-test, scale by view, always fullbright and oriented.
} SpriteType_t;

#ifdef __cplusplus

class ISprite
{
public:
	virtual void SetPosition(float x, float y, float z) = 0;
	virtual void SetPosition(PLVector3D position) = 0;
	virtual void SetColour(float r, float g, float b, float a = 1.0f) = 0;
	virtual void SetType(SpriteType_t type) = 0;
	virtual void SetActive(bool active) = 0;
	virtual void SetScale(float scale) = 0;
	virtual void SetMaterial(Material *material) = 0;

	virtual bool IsVisible() = 0;
	virtual bool IsActive() = 0;
	virtual bool IsLit() = 0;

	virtual float GetScale() = 0;
	virtual int GetIdentifier() = 0;
	virtual SpriteType_t GetType() = 0;
};

#else

typedef struct ISprite ISprite;
typedef struct Sprite Sprite;

#endif