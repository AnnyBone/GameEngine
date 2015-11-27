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

#include "engine_base.h"

#include "video.h"
#include "EngineVideoShadow.h"

/*
	Contains all basic drawing routines.
	TODO:
		Rename to engine_draw!
*/

#define	SHADOW_MAP_RESOLUTION	512	// TODO: Make this a console variable?

#define	SHADOW_BLOB_SCALE	20.0f	// Default blob scale.

extern MathVector3f_t lightspot;
extern mplane_t	*lightplane;		// Plane underneath the entity.

/*	Draws a simple rectangular blob under an entity.
	TODO:
		Fade out blob shadow based on distance from ground.
*/
void Shadow_DrawBlob(ClientEntity_t *Entity)
{
	if (!cvVideoDrawShadowBlob.bValue)
		return;

	// TODO: We ONLY want the bottom plane! Not the light sample... Simplify this?
	Light_GetSample(Entity->origin);
	if (!lightplane)
		return;

	VideoObjectVertex_t	voShadow[4] = { { { 0 } } };
	float fBlobHeight, fShadowScale[2];

	// TODO: This should be averaged out to the size of an entity.
	fShadowScale[0] = fShadowScale[1] = SHADOW_BLOB_SCALE;

	fBlobHeight = Entity->origin[2] - lightspot[2];

	Video_ResetCapabilities(false);

	VideoLayer_PushMatrix();

	Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);

	glTranslatef(Entity->origin[0], Entity->origin[1], Entity->origin[2]);
	glTranslatef(0, 0, -fBlobHeight + 0.1f);

	Video_ObjectVertex(&voShadow[0], -fShadowScale[0], fShadowScale[1], 0);
	Video_ObjectTexture(&voShadow[0], VIDEO_TEXTURE_DIFFUSE, 0, 0);
	Video_ObjectColour(&voShadow[0], 1.0f, 1.0f, 1.0f, 1.0f);

	Video_ObjectVertex(&voShadow[1], fShadowScale[0], fShadowScale[1], 0);
	Video_ObjectTexture(&voShadow[1], VIDEO_TEXTURE_DIFFUSE, 1.0f, 0);
	Video_ObjectColour(&voShadow[1], 1.0f, 1.0f, 1.0f, 1.0f);

	Video_ObjectVertex(&voShadow[2], fShadowScale[0], -fShadowScale[1], 0);
	Video_ObjectTexture(&voShadow[2], VIDEO_TEXTURE_DIFFUSE, 1.0f, 1.0f);
	Video_ObjectColour(&voShadow[2], 1.0f, 1.0f, 1.0f, 1.0f);

	Video_ObjectVertex(&voShadow[3], -fShadowScale[0], -fShadowScale[1], 0);
	Video_ObjectTexture(&voShadow[3], VIDEO_TEXTURE_DIFFUSE, 0, 1.0f);
	Video_ObjectColour(&voShadow[3], 1.0f, 1.0f, 1.0f, 1.0f);

	Video_DrawFill(voShadow, g_mBlobShadow, 0);

	glTranslatef(0, 0, fBlobHeight + 0.1);

	VideoLayer_PopMatrix();

	Video_ResetCapabilities(true);
}

void Shadow_DrawMap(ClientEntity_t *Entity)
{
	if (!cvVideoDrawShadowMap.bValue)
		return;

	DynamicLight_t *dlNearest = Light_GetDynamic(Entity->origin, false);
	if (dlNearest)
	{
		// TODO: Load up light protection matrix...

		VideoLayer_PushMatrix();
		VideoLayer_PopMatrix();
	}
}

/*	Draw multiple shadow types.
	TODO:
		Shadow maps.
		Need a flag, to disable/enable shadows on certain entities.
*/
#if 1
void Shadow_Draw(ClientEntity_t *Entity)
{
	// Only meshes are valid here.
	if ((Entity->model->mType == MODEL_TYPE_LEVEL) || (Entity->model->mType == MODEL_TYPE_SPRITE))
		return;

	// Make sure the entity is actually visible.
	if ((Entity == &cl.viewent) || (ENTALPHA_DECODE(Entity->alpha) <= 0) || R_CullModelForEntity(Entity))
		return;

	// Shadow blob
	Shadow_DrawBlob(Entity);

	// Shadow map
	Shadow_DrawMap(Entity);
#else	// Old shadows...
void Shadow_Draw(ClientEntity_t *ent)
{
	lerpdata_t		lerpdata;
	float			fShadowMatrix[16] =
	{	1, 0, 0,	0,
		0, 1, 0,	0,
		0, 0, 0,	0,
		0, 0, 0.1f, 1 };
	float			lheight, fShadowScale[2],
		fShadowAlpha = 0;

	if (ENTALPHA_DECODE(ent->alpha) <= 0)
		return;

	fShadowScale[0] = fShadowScale[1] = 20.0f;

	if (ent == &cl.viewent || R_CullModelForEntity(ent) || (!fShadowScale[0] || !fShadowScale[1]))
		return;

	// Allow us to cast shadows from entities that use bmodels. ~hogsy
	// TODO: Add a flag for this, rather than checking a string everytime... ~hogsy
	if (!strstr(ent->model->name, ".") || !strstr(ent->model->name, "/"))
		return;

	Light_GetSample(ent->origin);

	lheight = ent->origin[2] - lightspot[2];

	{
		/*	TODO:
			Trace down to get plane and set angles to that
			clip based on surface extents?
		*/
		VideoObjectVertex_t	voShadow[4] = { { { 0 } } };

		Video_ResetCapabilities(false);

		glPushMatrix();

		Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);

		glTranslatef(ent->origin[0], ent->origin[1], ent->origin[2]);
		glTranslatef(0, 0, -lheight + 0.1f);

		Video_ObjectVertex(&voShadow[0], -fShadowScale[0], fShadowScale[1], 0);
		Video_ObjectTexture(&voShadow[0], VIDEO_TEXTURE_DIFFUSE, 0, 0);
		Video_ObjectColour(&voShadow[0], 1.0f, 1.0f, 1.0f, 1.0f);

		Video_ObjectVertex(&voShadow[1], fShadowScale[0], fShadowScale[1], 0);
		Video_ObjectTexture(&voShadow[1], VIDEO_TEXTURE_DIFFUSE, 1.0f, 0);
		Video_ObjectColour(&voShadow[1], 1.0f, 1.0f, 1.0f, 1.0f);

		Video_ObjectVertex(&voShadow[2], fShadowScale[0], -fShadowScale[1], 0);
		Video_ObjectTexture(&voShadow[2], VIDEO_TEXTURE_DIFFUSE, 1.0f, 1.0f);
		Video_ObjectColour(&voShadow[2], 1.0f, 1.0f, 1.0f, 1.0f);

		Video_ObjectVertex(&voShadow[3], -fShadowScale[0], -fShadowScale[1], 0);
		Video_ObjectTexture(&voShadow[3], VIDEO_TEXTURE_DIFFUSE, 0, 1.0f);
		Video_ObjectColour(&voShadow[3], 1.0f, 1.0f, 1.0f, 1.0f);

		Video_DrawFill(voShadow, g_mBlobShadow, 0);

		glTranslatef(0, 0, lheight + 0.1);
		glPopMatrix();

		Video_ResetCapabilities(true);
	}

	// Player doesn't get animated, so don't bother with planar shadows for him.
	if (ent != &cl_entities[cl.viewentity])
		if ((r_shadows.value >= 2) && (ent->model->mType == MODEL_TYPE_MD2))
		{
			MD2_t			*pmd2;
			DynamicLight_t	*dlLight;
			vec3_t			vDistance;

			pmd2 = (MD2_t*)Mod_Extradata(ent->model);

			Alias_SetupFrame(pmd2, currententity, &lerpdata);

			bShading = false;

			fShadowMatrix[8] =
				fShadowMatrix[9] = 0;

			dlLight = Light_GetDynamic(ent->origin, false);
			if (!dlLight)
				return;

			Math_VectorSubtract(ent->origin, dlLight->origin, vDistance);

			fShadowAlpha = (dlLight->radius - Math_Length(vDistance)) / 100.0f;
			if (fShadowAlpha <= 0)
				return;

			fShadowMatrix[8] = vDistance[0] / 100.0f;
			fShadowMatrix[9] = vDistance[1] / 100.0f;

			glPushMatrix();

			Video_ResetCapabilities(false);

			Video_EnableCapabilities(VIDEO_BLEND | VIDEO_STENCIL_TEST);
			Video_DisableCapabilities(VIDEO_TEXTURE_2D);
			Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);

			glStencilFunc(GL_EQUAL, 1, 2);
			Video_SetColour(0, 0, 0, fShadowAlpha);
			glTranslatef(ent->origin[0], ent->origin[1], ent->origin[2]);
			glTranslatef(0, 0, -lheight);
			glMultMatrixf(fShadowMatrix);
			glTranslatef(0, 0, lheight);
			glRotatef(ent->angles[1], 0, 0, 1);
			glRotatef(ent->angles[0], 0, 1, 0);
			glRotatef(ent->angles[2], 1, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

			Alias_DrawFrame(pmd2, ent, lerpdata);

			glPopMatrix();

			Video_ResetCapabilities(true);
		}
#endif
}
