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

#include "../engine_base.h"

#include "../video.h"		// TODO: make this a base include
#include "effect_sprite.h"

ConsoleVariable_t cvSpriteDebugSize = { "sprite_debugsize", "0", false, false, "If enabled, shows the area that the sprite covers." };

SpriteManager *g_spritemanager;

// Sprite Manager

#define	SPRITE_DEFAULT_MAX	1024

SpriteManager::SpriteManager()
{
}

void SpriteManager::Initialize()
{
	Con_Printf("Initializing Sprite Manager...\n");

	Cvar_RegisterVariable(&cvSpriteDebugSize, NULL);

	sprites.reserve(SPRITE_DEFAULT_MAX);
}

/*	Add a new sprite to the manager.
*/
Sprite *SpriteManager::Add()
{
	Sprite *sprite = new Sprite();
	sprites.push_back(sprite);
	return sprite;
}

/*	Run through and simulate each sprite individually.
*/
void SpriteManager::Simulate()
{
	unsigned int i;
	for (i = 0; i < sprites.size(); i++)
		sprites[i]->Simulate();
}

void SpriteManager::Draw()
{
	unsigned int i;
	for (i = 0; i < sprites.size(); i++)
	{
		// Skip sprites that aren't currently visible.
		if (!sprites[i]->IsVisible())
			continue;

		sprites[i]->Draw();
	}
}

void SpriteManager::Shutdown()
{
	Con_Printf("Shutting down Sprite Manager...\n");

	sprites.clear();
}

/*
	Sprite
*/

Sprite::Sprite()
{
	ident		= 0;
	isactive	= false;
	islit		= false;
	isvisible	= false;
	scale		= 1.0f;
	material	= g_mMissingMaterial;

	Math_VectorClear(position);
	Math_VectorClear(mins);
	Math_VectorClear(maxs);
	Math_Vector4Set(1.0f, colour);
}

Sprite::~Sprite()
{
}

void Sprite::SetColour(float r, float g, float b, float a)
{
	colour[0] = r;
	colour[1] = g;
	colour[2] = b;
	colour[3] = a;
}

void Sprite::SetPosition(MathVector3f_t nposition)
{
	// Just use the other func.
	SetPosition(nposition[0], nposition[1], nposition[2]);
}

void Sprite::SetPosition(float x, float y, float z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void Sprite::SetType(SpriteType_t stype)
{
	// Don't bother if we're already that type.
	if (type == stype)
		return;

	// Otherwise update, and set defaults.
	type = stype;
	switch (type)
	{
	default:
	case SPRITE_TYPE_DEFAULT:
		// Otherwise by default, clipped by actual scale.
		Math_VectorSet(-16.0f, mins);
		Math_VectorSet(16.0f, maxs);
		break;
	case SPRITE_TYPE_FLARE:
	case SPRITE_TYPE_SCALE:
		// Flares get clipped by origin position, so no additional size.
		Math_VectorClear(mins);
		Math_VectorClear(maxs);
		break;
	}
}

void Sprite::SetScale(float nscale)
{
	// Little bit silly, sometimes we have an invalid scale.
	if (nscale <= 0)
		nscale = 1.0f;

	scale = nscale;
}

void Sprite::SetMaterial(Material_t *nmaterial)
{
	if (!nmaterial)
		return;

	material = nmaterial;
}

void Sprite::Simulate()
{
	// TODO: last bit is a hack, since having the console open doesn't count as being paused...
	if (!isactive || cl.bIsPaused || ((key_dest == key_console) && (svs.maxclients == 1)))
		return;

	isvisible = true;

	// Ensure it's on screen.
	if ((colour[3] <= 0) || R_CullBox(mins, maxs))
		isvisible = false;

	// Simulation depends on type, nothing complex though.
	switch (type)
	{
	default:
	case SPRITE_TYPE_DEFAULT:
		if (islit)
			Math_MVToVector(Light_GetSample(position), colour);
		break;
	case SPRITE_TYPE_FLARE:
	case SPRITE_TYPE_SCALE:
		// Scale the sprite, dependant on view position.
		scale *=
			(position[0] - r_origin[0])*vpn[0] +
			(position[1] - r_origin[1])*vpn[1] +
			(position[2] - r_origin[2])*vpn[2];
		break;
	}
}

void Sprite::Draw()
{
	// Not visible.
	if (!isvisible)
		return;

	if (colour[3] < 1.0f)
		vlEnable(VIDEO_BLEND);

#if 0	// TODO
	// Draw it via the VideoObject interface.
	vertexobj->Draw();
#endif

	if (colour[3] < 1.0f)
		vlDisable(VIDEO_BLEND);

	if (cvSpriteDebugSize.bValue)
	{
		// We need the size relative to the current position.
		MathVector3f_t NewMins, NewMaxs;
		Math_VectorAdd(mins, position, NewMins);
		Math_VectorAdd(maxs, position, NewMaxs);

		// Draw a point representing the current position.
		R_EmitWirePoint(position);

		// Draw the bounding box.
		R_EmitWireBox(NewMins, NewMaxs, 0, 1.0f, 0);
	}
}

/*
	C Interface
*/

void Sprite_DrawSimple(Material_t *material, MathVector3f_t position, float scale)
{
	// Create the new sprite.
	Sprite *rendersprite = new Sprite();

	// Set it up.
	rendersprite->SetActive(true);
	rendersprite->SetColour(1.0f, 1.0f, 1.0f, 0.5f);
	rendersprite->SetPosition(position);
	rendersprite->SetScale(scale);
	rendersprite->SetMaterial(material);

	// Simulate it and then draw.
	rendersprite->Simulate();
	rendersprite->Draw();

	// Delete it.
	delete rendersprite;
}

/**/
