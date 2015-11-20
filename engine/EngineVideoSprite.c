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

#include "EngineBase.h"

#include "video.h"

/*
	Sprite Management
*/

typedef enum
{
	SPRITE_TYPE_DEFAULT,	// Depth-test and scaled manually.
	SPRITE_TYPE_FLARE		// No depth-test, scale by view and always fullbright.
} SpriteType_t;

typedef struct Sprite_s
{
	SpriteType_t stType;	// Type of sprite (see SpriteType_t).

	MathVector3f_t vOrigin;

	MathVector4f_t vColour;	// Sprite colour.

	bool bLit;				// If set to true, the sprite is effected by lighting.

	Material_t mMaterial;	// Assigned material.

	float fScale;			// Sprite scale.

	struct Sprite_s *sNext;	// Next sprite in list.
} Sprite_t;

Sprite_t *sSprites,
		 *sActiveSprites,
		 *sFreeSprites;

/*  Allocate a new sprite.
*/
Sprite_t *Sprite_Allocate(void)
{
	Sprite_t *sAllocated;

	if(!sFreeSprites)
		return NULL;

	sAllocated			= sFreeSprites;
	sFreeSprites		= sAllocated->sNext;
	sAllocated->sNext	= sActiveSprites;
	sActiveSprites		= sAllocated;

#if 0
	// [22/5/2014] Clear it out... ~hogsy
	memset(sAllocated,0,sizeof(sAllocated));
#else
	Math_Vector4Set(1.0f,sAllocated->vColour);
#endif

	return sAllocated;
}

void Sprite_Process(void)
{
	Sprite_t *sSprite;

	if(!sActiveSprites)
		return;

	for(sSprite = sActiveSprites; sSprite; sSprite = sSprite->sNext)
	{
		if(sSprite->bLit && (sSprite->stType != SPRITE_TYPE_FLARE))
			Math_MVToVector(Light_GetSample(sSprite->vOrigin),sSprite->vColour);

		switch(sSprite->stType)
		{
		case SPRITE_TYPE_DEFAULT:
			break;
		case SPRITE_TYPE_FLARE:
			// [22/5/2014] Scale by view ~hogsy
			sSprite->fScale *=
				(sSprite->vOrigin[0]-r_origin[0])*vpn[0]+
				(sSprite->vOrigin[1]-r_origin[1])*vpn[1]+
				(sSprite->vOrigin[2]-r_origin[2])*vpn[2];
		}
	}
}

void Sprite_Draw(ClientEntity_t *eEntity)
{
	Sprite_t *sSprite;

	Video_ResetCapabilities(false);
	Video_DisableCapabilities(VIDEO_DEPTH_TEST);
	Video_EnableCapabilities(VIDEO_BLEND);

	for(sSprite = sActiveSprites; sSprite; sSprite = sSprite->sNext)
	{
		VideoObjectVertex_t voSprite[4];

		Video_ObjectTexture(&voSprite[0], 0, 0, 0);
		Video_ObjectTexture(&voSprite[1], 0, 1.0f, 0);
		Video_ObjectTexture(&voSprite[2], 0, 1.0f, 1.0f);
		Video_ObjectTexture(&voSprite[3], 0, 0, 1.0f);

		Video_ObjectColour(&voSprite[0], sSprite->vColour[0], sSprite->vColour[1], sSprite->vColour[2], sSprite->vColour[3]);
		Video_ObjectColour(&voSprite[1], sSprite->vColour[0], sSprite->vColour[1], sSprite->vColour[2], sSprite->vColour[3]);
		Video_ObjectColour(&voSprite[2], sSprite->vColour[0], sSprite->vColour[1], sSprite->vColour[2], sSprite->vColour[3]);
		Video_ObjectColour(&voSprite[3], sSprite->vColour[0], sSprite->vColour[1], sSprite->vColour[2], sSprite->vColour[3]);

		Math_VectorCopy(sSprite->vOrigin, voSprite[0].mvPosition);
		Math_VectorMA(sSprite->vOrigin, sSprite->fScale, vup, voSprite[1].mvPosition);
		Math_VectorMA(voSprite[1].mvPosition, sSprite->fScale, vright, voSprite[2].mvPosition);
		Math_VectorMA(sSprite->vOrigin, sSprite->fScale, vright, voSprite[3].mvPosition);
		
		//Video_DrawFill(voSprite);
	}

	Video_ResetCapabilities(true);
}
