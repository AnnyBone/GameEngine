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

extern SpriteManager *g_spritemanager;

#else

Sprite *SpriteManager_Add(void);

void SpriteManager_Remove(Sprite *sprite);
void SpriteManager_Clear(void);
void SpriteManager_Simulate(void);
void SpriteManager_Draw(void);

void Sprite_SetPosition(Sprite *sprite, MathVector3f_t position);
void Sprite_SetColour(Sprite *sprite, float r, float g, float b, float a);
void Sprite_SetType(Sprite *sprite, SpriteType_t type);
void Sprite_SetScale(Sprite *sprite, float scale);
void Sprite_SetMaterial(Sprite *sprite, Material_t *material);

#endif