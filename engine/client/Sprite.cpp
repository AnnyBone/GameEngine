/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

#include "../EngineBase.h"

#include "../EngineVideo.h"

ConsoleVariable_t cvSpriteDebugSize = { "sprite_debugsize", "0", false, false, "If enabled, shows the area that the sprite covers." };

typedef enum
{
	SPRITE_TYPE_DEFAULT,	// Depth-test, scaled manually and oriented.
	SPRITE_TYPE_SCALE,		// Depth-test, scale by view and oriented.
	SPRITE_TYPE_FLARE		// No depth-test, scale by view, always fullbright and oriented.
} SpriteType_t;

class CSprite
{
public:
	CSprite();
	virtual ~CSprite();

	void SetPosition(float X, float Y, float Z);
	void SetColour(float R, float G, float B);
	void SetType(SpriteType_t InType);
	void SetActive(bool bValue) { bActive = bValue; }
	void Simulate();

	virtual void Draw();

	bool IsVisible() { return bVisible; }
	bool IsActive()	{ return bActive; }
	bool IsLit() { return bLit; }

private:
	float fScale;

	bool
		bLit,
		bVisible,
		bBlend,
		bActive;

	MathVector3f_t
		Position,
		Mins, Maxs;

	SpriteType_t Type;

	Colour_t Colour;

	VideoSys::CVideoObject *RenderObject;

	Material_t *Material;
};

class CSpriteManager
{
public:
	CSpriteManager();

	CSprite *Add();

	void Initialize();
	void Simulate();
	void Draw();
	void Shutdown();

private:
	CSprite **Sprites;
};

// Sprite Manager

CSpriteManager::CSpriteManager()
{
	Sprites = NULL;
}

void CSpriteManager::Initialize()
{
	Con_Printf("Initializing Sprite Manager...\n");

	Cvar_RegisterVariable(&cvSpriteDebugSize, NULL);

	Sprites = new CSprite*;
}

/*	Add a new sprite to the manager.
*/
CSprite *CSpriteManager::Add()
{
	CSprite *NewSprite = new CSprite();
	// TODO: Add to our list.
	return NewSprite;
}

void CSpriteManager::Simulate()
{
	CSprite *Sprite = Sprites[0];
	while (Sprite)
	{
		Sprite->Simulate();

		Sprite++;
	}
}

void CSpriteManager::Draw()
{
	CSprite *CurrentSprite = Sprites[0];
	while (CurrentSprite)
	{
		// Skip sprites that aren't currently visible.
		if (!CurrentSprite->IsVisible())
			continue;

		CurrentSprite->Draw();
		CurrentSprite++;
	}
}

void CSpriteManager::Shutdown()
{
	Con_Printf("Shutting down Sprite Manager...\n");

	delete *Sprites;
}

// Sprite

CSprite::CSprite()
{
	bActive = false;
	bLit = false;
	bVisible = false;
	bBlend = false;

	fScale = 1.0f;

	Material = mNoTexture;

	Math_VectorClear(Position);
	Math_VectorClear(Mins);
	Math_VectorClear(Maxs);
	Math_Vector4Set(1.0f, Colour);
}

CSprite::~CSprite()
{
}

void CSprite::SetColour(float R, float G, float B)
{
	Colour[0] = R;
	Colour[1] = G;
	Colour[2] = B;
}

void CSprite::SetPosition(float X,float Y,float Z)
{
	Position[0] = X;
	Position[1] = Y;
	Position[2] = Z;
}

void CSprite::SetType(SpriteType_t InType)
{
	switch (Type)
	{
	default:
	case SPRITE_TYPE_DEFAULT:
		Math_VectorClear(Mins);
		Math_VectorClear(Maxs);
		break;
	case SPRITE_TYPE_FLARE:
	case SPRITE_TYPE_SCALE:
		Mins[0] = Mins[1] = Mins[2] = -2;
		Maxs[0] = Maxs[1] = Maxs[2] = 2;
		break;
	}
}

void CSprite::Simulate()
{
	if (cl.bIsPaused || ((key_dest == key_console) && (svs.maxclients == 1)))
		return;

	// Ensure it's on screen.
	if ((Colour[3] <= 0) || R_CullBox(Mins, Maxs))
		bVisible = false;

	// Ensure it's not invisible.
	if (Colour[3] < 1.0f)
	{
		bVisible = true;
		bBlend = true;
	}
	else
	{
		bBlend = false;
	}

	// Simulation depends on type, nothing complex though.
	switch (Type)
	{
	default:
	case SPRITE_TYPE_DEFAULT:
		break;
	case SPRITE_TYPE_FLARE:
		break;
	case SPRITE_TYPE_SCALE:
		break;
	}
}

void CSprite::Draw()
{
	// Not visible.
	if (!bVisible)
		return;

	if (bBlend)
		VideoLayer_Enable(VIDEO_BLEND);

	// Draw it via the VideoObject interface.
	RenderObject->Draw();

	if (bBlend)
		VideoLayer_Disable(VIDEO_BLEND);

	if (cvSpriteDebugSize.bValue)
	{
		// We need the size relative to the current position.
		MathVector3f_t NewMins, NewMaxs;
		Math_VectorAdd(Mins, Position, NewMins);
		Math_VectorAdd(Maxs, Position, NewMaxs);

		// Draw a point representing the current position.
		R_EmitWirePoint(Position);

		// Draw the bounding box.
		R_EmitWireBox(NewMins, NewMaxs, 0, 1.0f, 0);
	}
}
