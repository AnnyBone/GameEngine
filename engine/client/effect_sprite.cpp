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

#include "../engine_base.h"

#include "../video.h"

ConsoleVariable_t cvSpriteDebugSize = { "sprite_debugsize", "0", false, false, "If enabled, shows the area that the sprite covers." };

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

	VideoVertexObject *vertexobj;

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
	int i;
	for (i = 0; i < sprites.size; i++)
		sprites[i]->Simulate();
}

void SpriteManager::Draw()
{
	int i;
	for (i = 0; i < sprites.size; i++)
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
	isactive = false;
	islit = false;
	isvisible = false;

	scale = 1.0f;

	material = g_mMissingMaterial;

	Math_VectorClear(Position);
	Math_VectorClear(mins);
	Math_VectorClear(maxs);
	Math_Vector4Set(1.0f, Colour);

	vertexobj = new VideoVertexObject();
}

Sprite::~Sprite()
{
	delete vertexobj;
}

void Sprite::SetColour(float r, float g, float b, float a)
{
	Colour[0] = r;
	Colour[1] = g;
	Colour[2] = b;
	Colour[3] = a;
}

void Sprite::SetPosition(MathVector3f_t position)
{
	// Just use the other func.
	SetPosition(position[0], position[1], position[2]);
}

void Sprite::SetPosition(float x, float y, float z)
{
	Position[0] = x;
	Position[1] = y;
	Position[2] = z;
}

void Sprite::SetType(SpriteType_t type)
{
	// Don't bother if we're already that type.
	if (Type == type)
		return;

	// Otherwise update, and set defaults.
	Type = type;
	switch (Type)
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

void Sprite::SetScale(float scale)
{
	// Little bit silly, sometimes we have an invalid scale.
	if (scale <= 0)
		scale = 1.0f;

	this->scale = scale;
}

void Sprite::Simulate()
{
	// TODO: last bit is a hack, since having the console open doesn't count as being paused...
	if (!isactive || cl.bIsPaused || ((key_dest == key_console) && (svs.maxclients == 1)))
		return;

	isvisible = true;

	// Ensure it's on screen.
	if ((Colour[3] <= 0) || R_CullBox(mins, maxs))
		isvisible = false;

	// Simulation depends on type, nothing complex though.
	switch (Type)
	{
	default:
	case SPRITE_TYPE_DEFAULT:
		break;
	case SPRITE_TYPE_FLARE:
	case SPRITE_TYPE_SCALE:
		// Scale the sprite, dependant on view position.
		scale *=
			(Position[0] - r_origin[0])*vpn[0] +
			(Position[1] - r_origin[1])*vpn[1] +
			(Position[2] - r_origin[2])*vpn[2];
		break;
	}
}

void Sprite::Draw()
{
	// Not visible.
	if (!isvisible)
		return;

	if (Colour[3] < 1.0f)
		VideoLayer_Enable(VIDEO_BLEND);

	// Draw it via the VideoObject interface.
	vertexobj->Draw();

	if (Colour[3] < 1.0f)
		VideoLayer_Disable(VIDEO_BLEND);

	if (cvSpriteDebugSize.bValue)
	{
		// We need the size relative to the current position.
		MathVector3f_t NewMins, NewMaxs;
		Math_VectorAdd(mins, Position, NewMins);
		Math_VectorAdd(maxs, Position, NewMaxs);

		// Draw a point representing the current position.
		R_EmitWirePoint(Position);

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
