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

#include "EngineVideo.h"
#include "EngineVideoShader.h"

extern cvar_t r_drawflat, gl_fullbrights, r_lerpmodels, r_lerpmove; //johnfitz

#define NUMVERTEXNORMALS	162

MathVector3_t	r_avertexnormals[NUMVERTEXNORMALS] =
{
	{ -0.525731, 0.000000, 0.850651 },
	{ -0.442863, 0.238856, 0.864188 },
	{ -0.295242, 0.000000, 0.955423 },
	{ -0.309017, 0.500000, 0.809017 },
	{ -0.162460, 0.262866, 0.951056 },
	{ 0.000000, 0.000000, 1.000000 },
	{ 0.000000, 0.850651, 0.525731 },
	{ -0.147621, 0.716567, 0.681718 },
	{ 0.147621, 0.716567, 0.681718 },
	{ 0.000000, 0.525731, 0.850651 },
	{ 0.309017, 0.500000, 0.809017 },
	{ 0.525731, 0.000000, 0.850651 },
	{ 0.295242, 0.000000, 0.955423 },
	{ 0.442863, 0.238856, 0.864188 },
	{ 0.162460, 0.262866, 0.951056 },
	{ -0.681718, 0.147621, 0.716567 },
	{ -0.809017, 0.309017, 0.500000 },
	{ -0.587785, 0.425325, 0.688191 },
	{ -0.850651, 0.525731, 0.000000 },
	{ -0.864188, 0.442863, 0.238856 },
	{ -0.716567, 0.681718, 0.147621 },
	{ -0.688191, 0.587785, 0.425325 },
	{ -0.500000, 0.809017, 0.309017 },
	{ -0.238856, 0.864188, 0.442863 },
	{ -0.425325, 0.688191, 0.587785 },
	{ -0.716567, 0.681718, -0.147621 },
	{ -0.500000, 0.809017, -0.309017 },
	{ -0.525731, 0.850651, 0.000000 },
	{ 0.000000, 0.850651, -0.525731 },
	{ -0.238856, 0.864188, -0.442863 },
	{ 0.000000, 0.955423, -0.295242 },
	{ -0.262866, 0.951056, -0.162460 },
	{ 0.000000, 1.000000, 0.000000 },
	{ 0.000000, 0.955423, 0.295242 },
	{ -0.262866, 0.951056, 0.162460 },
	{ 0.238856, 0.864188, 0.442863 },
	{ 0.262866, 0.951056, 0.162460 },
	{ 0.500000, 0.809017, 0.309017 },
	{ 0.238856, 0.864188, -0.442863 },
	{ 0.262866, 0.951056, -0.162460 },
	{ 0.500000, 0.809017, -0.309017 },
	{ 0.850651, 0.525731, 0.000000 },
	{ 0.716567, 0.681718, 0.147621 },
	{ 0.716567, 0.681718, -0.147621 },
	{ 0.525731, 0.850651, 0.000000 },
	{ 0.425325, 0.688191, 0.587785 },
	{ 0.864188, 0.442863, 0.238856 },
	{ 0.688191, 0.587785, 0.425325 },
	{ 0.809017, 0.309017, 0.500000 },
	{ 0.681718, 0.147621, 0.716567 },
	{ 0.587785, 0.425325, 0.688191 },
	{ 0.955423, 0.295242, 0.000000 },
	{ 1.000000, 0.000000, 0.000000 },
	{ 0.951056, 0.162460, 0.262866 },
	{ 0.850651, -0.525731, 0.000000 },
	{ 0.955423, -0.295242, 0.000000 },
	{ 0.864188, -0.442863, 0.238856 },
	{ 0.951056, -0.162460, 0.262866 },
	{ 0.809017, -0.309017, 0.500000 },
	{ 0.681718, -0.147621, 0.716567 },
	{ 0.850651, 0.000000, 0.525731 },
	{ 0.864188, 0.442863, -0.238856 },
	{ 0.809017, 0.309017, -0.500000 },
	{ 0.951056, 0.162460, -0.262866 },
	{ 0.525731, 0.000000, -0.850651 },
	{ 0.681718, 0.147621, -0.716567 },
	{ 0.681718, -0.147621, -0.716567 },
	{ 0.850651, 0.000000, -0.525731 },
	{ 0.809017, -0.309017, -0.500000 },
	{ 0.864188, -0.442863, -0.238856 },
	{ 0.951056, -0.162460, -0.262866 },
	{ 0.147621, 0.716567, -0.681718 },
	{ 0.309017, 0.500000, -0.809017 },
	{ 0.425325, 0.688191, -0.587785 },
	{ 0.442863, 0.238856, -0.864188 },
	{ 0.587785, 0.425325, -0.688191 },
	{ 0.688191, 0.587785, -0.425325 },
	{ -0.147621, 0.716567, -0.681718 },
	{ -0.309017, 0.500000, -0.809017 },
	{ 0.000000, 0.525731, -0.850651 },
	{ -0.525731, 0.000000, -0.850651 },
	{ -0.442863, 0.238856, -0.864188 },
	{ -0.295242, 0.000000, -0.955423 },
	{ -0.162460, 0.262866, -0.951056 },
	{ 0.000000, 0.000000, -1.000000 },
	{ 0.295242, 0.000000, -0.955423 },
	{ 0.162460, 0.262866, -0.951056 },
	{ -0.442863, -0.238856, -0.864188 },
	{ -0.309017, -0.500000, -0.809017 },
	{ -0.162460, -0.262866, -0.951056 },
	{ 0.000000, -0.850651, -0.525731 },
	{ -0.147621, -0.716567, -0.681718 },
	{ 0.147621, -0.716567, -0.681718 },
	{ 0.000000, -0.525731, -0.850651 },
	{ 0.309017, -0.500000, -0.809017 },
	{ 0.442863, -0.238856, -0.864188 },
	{ 0.162460, -0.262866, -0.951056 },
	{ 0.238856, -0.864188, -0.442863 },
	{ 0.500000, -0.809017, -0.309017 },
	{ 0.425325, -0.688191, -0.587785 },
	{ 0.716567, -0.681718, -0.147621 },
	{ 0.688191, -0.587785, -0.425325 },
	{ 0.587785, -0.425325, -0.688191 },
	{ 0.000000, -0.955423, -0.295242 },
	{ 0.000000, -1.000000, 0.000000 },
	{ 0.262866, -0.951056, -0.162460 },
	{ 0.000000, -0.850651, 0.525731 },
	{ 0.000000, -0.955423, 0.295242 },
	{ 0.238856, -0.864188, 0.442863 },
	{ 0.262866, -0.951056, 0.162460 },
	{ 0.500000, -0.809017, 0.309017 },
	{ 0.716567, -0.681718, 0.147621 },
	{ 0.525731, -0.850651, 0.000000 },
	{ -0.238856, -0.864188, -0.442863 },
	{ -0.500000, -0.809017, -0.309017 },
	{ -0.262866, -0.951056, -0.162460 },
	{ -0.850651, -0.525731, 0.000000 },
	{ -0.716567, -0.681718, -0.147621 },
	{ -0.716567, -0.681718, 0.147621 },
	{ -0.525731, -0.850651, 0.000000 },
	{ -0.500000, -0.809017, 0.309017 },
	{ -0.238856, -0.864188, 0.442863 },
	{ -0.262866, -0.951056, 0.162460 },
	{ -0.864188, -0.442863, 0.238856 },
	{ -0.809017, -0.309017, 0.500000 },
	{ -0.688191, -0.587785, 0.425325 },
	{ -0.681718, -0.147621, 0.716567 },
	{ -0.442863, -0.238856, 0.864188 },
	{ -0.587785, -0.425325, 0.688191 },
	{ -0.309017, -0.500000, 0.809017 },
	{ -0.147621, -0.716567, 0.681718 },
	{ -0.425325, -0.688191, 0.587785 },
	{ -0.162460, -0.262866, 0.951056 },
	{ 0.442863, -0.238856, 0.864188 },
	{ 0.162460, -0.262866, 0.951056 },
	{ 0.309017, -0.500000, 0.809017 },
	{ 0.147621, -0.716567, 0.681718 },
	{ 0.000000, -0.525731, 0.850651 },
	{ 0.425325, -0.688191, 0.587785 },
	{ 0.587785, -0.425325, 0.688191 },
	{ 0.688191, -0.587785, 0.425325 },
	{ -0.955423, 0.295242, 0.000000 },
	{ -0.951056, 0.162460, 0.262866 },
	{ -1.000000, 0.000000, 0.000000 },
	{ -0.850651, 0.000000, 0.525731 },
	{ -0.955423, -0.295242, 0.000000 },
	{ -0.951056, -0.162460, 0.262866 },
	{ -0.864188, 0.442863, -0.238856 },
	{ -0.951056, 0.162460, -0.262866 },
	{ -0.809017, 0.309017, -0.500000 },
	{ -0.864188, -0.442863, -0.238856 },
	{ -0.951056, -0.162460, -0.262866 },
	{ -0.809017, -0.309017, -0.500000 },
	{ -0.681718, 0.147621, -0.716567 },
	{ -0.681718, -0.147621, -0.716567 },
	{ -0.850651, 0.000000, -0.525731 },
	{ -0.688191, 0.587785, -0.425325 },
	{ -0.587785, 0.425325, -0.688191 },
	{ -0.425325, 0.688191, -0.587785 },
	{ -0.425325, -0.688191, -0.587785 },
	{ -0.587785, -0.425325, -0.688191 },
	{ -0.688191, -0.587785, -0.425325 },
};

bool	bOverbright,
		bShading;		//johnfitz -- if false, disable vertex shading for various reasons (fullbright, r_lightmap, showtris, etc)

MathVector3f_t	vLightColour;

DynamicLight_t	*dlLightSource;

void Alias_SetupLighting(ClientEntity_t *ceEntity)
{
	float fDistance;

	if(!bShading)
		return;

	Math_VectorSet(1.0f, vLightColour);

	if (!(ceEntity->effects & EF_FULLBRIGHT))
	{
		// Check to see if we can grab the light source, for directional information.
		dlLightSource = Light_GetDynamic(ceEntity->origin, true);
		if (dlLightSource)
		{
			MathVector3f_t mvLightOut;

			Math_VectorScale(dlLightSource->color, 1.0f / 200.0f, mvLightOut);

			VideoShader_SetVariable3f("lightPosition", dlLightSource->origin[0], dlLightSource->origin[1], dlLightSource->origin[2]);
			VideoShader_SetVariable3f("lightColour", mvLightOut[0], mvLightOut[1], mvLightOut[2]);
		}
		else
		{
			VideoShader_SetVariable3f("lightPosition", 0, 0, 0);
			VideoShader_SetVariable3f("lightColour", 0, 0, 0);
		}
	}

	// Minimum light value on players (8)
	if (ceEntity > cl_entities && ceEntity <= cl_entities + cl.maxclients)
	{
		fDistance = 24.0f-(vLightColour[0]+vLightColour[1]+vLightColour[2]);
		if(fDistance > 0.0f)
			Math_VectorAddValue(vLightColour,fDistance/3.0f,vLightColour);
	}

	Math_VectorScale(vLightColour,1.0f/200.0f,vLightColour);
}

void Alias_DrawFrame(MD2_t *mModel,entity_t *eEntity,lerpdata_t lLerpData)
{
#if 0	// Broken
	int	i, j, k, iVert;
	float fAlpha;
	VideoObjectVertex_t *voModel;
	MD2TriangleVertex_t	*mtvVertices, *mtvLerpVerts;
	MD2Triangle_t *mtTriangles;
	MD2Frame_t *mfFirst, *mfSecond;
	MathVector3_t scale1, scale2;

	// Make sure that entity scale is valid.
	if(currententity->scale < 0.1f)
		currententity->scale = 1.0f;

	//new version by muff - fixes bug, easier to read, faster (well slightly)
	mfFirst		= (MD2Frame_t*)((uint8_t*)mModel+mModel->ofs_frames+(mModel->framesize*currententity->draw_lastpose));
	mfSecond	= (MD2Frame_t*)((uint8_t*)mModel+mModel->ofs_frames+(mModel->framesize*currententity->draw_pose));

	// Apply entity scaling for the model.
	Math_VectorScale(mfFirst->scale, currententity->scale, scale1);
	Math_VectorScale(mfSecond->scale, currententity->scale, scale2);

	fAlpha = ENTALPHA_DECODE(currententity->alpha);

	mtvVertices		= &mfFirst->verts[0];
	mtvLerpVerts	= &mfSecond->verts[0];

	voModel = (VideoObjectVertex_t*)Hunk_TempAlloc(mModel->num_glcmds*sizeof(VideoObjectVertex_t));

	mtTriangles	= (MD2Triangle_t*)((uint8_t*)mModel+mModel->ofs_tris);
	for(iVert = 0,i = 0; i < mModel->numtris; i++,mtTriangles++)
        for(k = 0; k < 3; k++)
        {
            for(j = 0; j < 3; j++)
            {
                voModel[iVert].vVertex[j] =	(mtvVertices[mtTriangles->index_xyz[k]].v[j]*scale1[j]+mfFirst->translate[j])*(1.0f-lLerpData.blend)+
											(mtvLerpVerts[mtTriangles->index_xyz[k]].v[j]*scale2[j]+mfSecond->translate[j])*lLerpData.blend;
				voModel[iVert].vColour[j] = 1.0f;
            }

			voModel[iVert].vTextureCoord[0][0] = (float)mModel->mtcTextureCoord[mtTriangles->index_st[k]].S / (float)mModel->skinwidth;
			voModel[iVert].vTextureCoord[0][1] = (float)mModel->mtcTextureCoord[mtTriangles->index_st[k]].T / (float)mModel->skinheight;

			voModel[iVert].vColour[3] = fAlpha;

			iVert++;
        }

	Video_DrawObject(voModel, VIDEO_PRIMITIVE_TRIANGLES, iVert, Material_Get(eEntity->model->mAssignedMaterials->iIdentification), eEntity->skinnum);
#else
	MD2TriangleVertex_t	*verts1, *verts2;
	MD2Frame_t *frame1, *frame2;

	float ilerp, fAlpha;
	unsigned int uiVerts = 0;
	int	*order, count;

	VideoObjectVertex_t *voModel;

	ilerp = 1.0f - lLerpData.blend;
	fAlpha = ENTALPHA_DECODE(eEntity->alpha);

	//new version by muff - fixes bug, easier to read, faster (well slightly)
	frame1 = (MD2Frame_t*)((uint8_t*)mModel + mModel->ofs_frames + (mModel->framesize*eEntity->draw_lastpose));
	frame2 = (MD2Frame_t*)((uint8_t*)mModel + mModel->ofs_frames + (mModel->framesize*eEntity->draw_pose));

	// TODO: do this via shader.
	if ((eEntity->scale != 1.0f) && (eEntity->scale > 0.1f))
		glScalef(eEntity->scale, eEntity->scale, eEntity->scale);

	verts1 = &frame1->verts[0];
	verts2 = &frame2->verts[0];

	order = (int*)((uint8_t*)mModel + mModel->ofs_glcmds);

	voModel = (VideoObjectVertex_t*)Hunk_TempAlloc(mModel->num_glcmds*sizeof(VideoObjectVertex_t));

	for (;;)
	{
		count = *order++;
		if (!count)
			break;		// done

		uiVerts = 0;

		do
		{
			Video_ObjectTexture(&voModel[uiVerts], VIDEO_TEXTURE_DIFFUSE, ((float*)order)[0], ((float*)order)[1]);
			Video_ObjectVertex(&voModel[uiVerts],
				(verts1[order[2]].v[0] * frame1->scale[0] + frame1->translate[0])*ilerp +
				(verts2[order[2]].v[0] * frame2->scale[0] + frame2->translate[0])*lLerpData.blend,
				(verts1[order[2]].v[1] * frame1->scale[1] + frame1->translate[1])*ilerp +
				(verts2[order[2]].v[1] * frame2->scale[1] + frame2->translate[1])*lLerpData.blend,
				(verts1[order[2]].v[2] * frame1->scale[2] + frame1->translate[2])*ilerp +
				(verts2[order[2]].v[2] * frame2->scale[2] + frame2->translate[2])*lLerpData.blend);
			Video_ObjectNormal(&voModel[uiVerts], 
				eEntity->model->object.ovVertices[uiVerts].mvNormal[0], 
				eEntity->model->object.ovVertices[uiVerts].mvNormal[1],
				eEntity->model->object.ovVertices[uiVerts].mvNormal[2]);

			if (bShading)
				Video_ObjectColour(&voModel[uiVerts],1.0f,1.0f,1.0f,fAlpha);
			else
				Video_ObjectColour(&voModel[uiVerts], 1.0f, 1.0f, 1.0f, 1.0f);

			uiVerts++;

			order += 3;
		} while (--count);

		Video_DrawObject(voModel, VIDEO_PRIMITIVE_TRIANGLE_FAN, uiVerts, eEntity->model->mAssignedMaterials, eEntity->skinnum);
	}
#endif
}

void Alias_SetupFrame(MD2_t *mModel,ClientEntity_t *ceCurrent,lerpdata_t *ldLerp)
{
	if ((ceCurrent->frame >= mModel->num_frames) || (ceCurrent->frame < 0))
	{
		Con_Warning("No such frame! (%d) (%s)\n", ceCurrent->frame, ceCurrent->model->name);

		ceCurrent->frame = 0;
	}

	// [13/9/2012] Added check for r_lerpmodels to solve issue ~hogsy
	if (r_lerpmodels.value >= 1 && ceCurrent->draw_lastmodel == ceCurrent->model)
	{
		if (ceCurrent->frame != ceCurrent->draw_pose)
		{
			ceCurrent->draw_lastpose = ceCurrent->draw_pose;
			ceCurrent->draw_pose = ceCurrent->frame;
			ceCurrent->draw_lerpstart = cl.time;

			ldLerp->blend = 0;
		}
		// [13/9/2012] Removed check for r_lerpmodels here since it's now done above ~hogsy
		else
			ldLerp->blend = (cl.time - ceCurrent->draw_lerpstart)*20.0;
	}
	else
	{
		ceCurrent->draw_lastmodel = ceCurrent->model;
		ceCurrent->draw_lastpose = ceCurrent->draw_pose = ceCurrent->frame;
		ceCurrent->draw_lerpstart = cl.time;

		ldLerp->blend = 0;
	}

	if(ldLerp->blend > 1.0f)
		ldLerp->blend = 1.0f;
}

void Alias_SetupEntityTransform(ClientEntity_t *ceEntity, lerpdata_t *lerpdata)
{
	float	blend;
	int		i;
	vec3_t	d;

	// if LERP_RESETMOVE, kill any lerps in progress
	if (ceEntity->lerpflags & LERP_RESETMOVE)
	{
		ceEntity->movelerpstart = 0;

		Math_VectorCopy(ceEntity->origin, ceEntity->previousorigin);
		Math_VectorCopy(ceEntity->origin, ceEntity->currentorigin);
		Math_VectorCopy(ceEntity->angles, ceEntity->previousangles);
		Math_VectorCopy(ceEntity->angles, ceEntity->currentangles);

		ceEntity->lerpflags -= LERP_RESETMOVE;
	}
	else if (!Math_VectorCompare(ceEntity->origin, ceEntity->currentorigin) || !Math_VectorCompare(ceEntity->angles, ceEntity->currentangles)) // origin/angles changed, start new lerp
	{
		ceEntity->movelerpstart = cl.time;

		Math_VectorCopy(ceEntity->currentorigin, ceEntity->previousorigin);
		Math_VectorCopy(ceEntity->origin, ceEntity->currentorigin);
		Math_VectorCopy(ceEntity->currentangles, ceEntity->previousangles);
		Math_VectorCopy(ceEntity->angles, ceEntity->currentangles);
	}

	//set up values
	if (r_lerpmove.value && ceEntity != &cl.viewent && ceEntity->lerpflags & LERP_MOVESTEP)
	{
		if (ceEntity->lerpflags & LERP_FINISH)
			blend = Math_Clamp(0, (cl.time - ceEntity->movelerpstart) / (ceEntity->lerpfinish - ceEntity->movelerpstart), 1);
		else
			blend = Math_Clamp(0, (cl.time - ceEntity->movelerpstart) / 0.1, 1);

		//translation
		Math_VectorSubtract(ceEntity->currentorigin, ceEntity->previousorigin, d);
		lerpdata->origin[0] = ceEntity->previousorigin[0] + d[0] * blend;
		lerpdata->origin[1] = ceEntity->previousorigin[1] + d[1] * blend;
		lerpdata->origin[2] = ceEntity->previousorigin[2] + d[2] * blend;

		//rotation
		Math_VectorSubtract(ceEntity->currentangles, ceEntity->previousangles, d);
		for(i = 0; i < 3; i++)
		{
			if (d[i] > 180.0f)
				d[i] -= 360.0f;
			if (d[i] < -180.0f)
				d[i] += 360.0f;
		}

		Math_VectorMA(ceEntity->previousangles, blend, d, lerpdata->angles);
	}
	else //don't lerp
	{
		Math_VectorCopy(ceEntity->origin, lerpdata->origin);
		Math_VectorCopy(ceEntity->angles, lerpdata->angles);
	}
}

// [23/8/2013] TODO: This seriously needs a damn header ~hogsy
void R_RotateForEntity(vec3_t origin,vec3_t angles);

/*	Draw the alias model.
*/
void Alias_Draw(ClientEntity_t *eEntity)
{
	lerpdata_t lLerpData;
	MD2_t *mModel;

	// [17/10/2013] Oops! Added this back in :) ~hogsy
	if(!cvVideoDrawModels.value || R_CullModelForEntity(eEntity))
		return;

	// [27/6/2013] Set defaults ~hogsy
	bShading = true;

	mModel = (MD2_t*)Mod_Extradata(eEntity->model);

	// [23/8/2013] Update alias poly count! ~hogsy
	rs_aliaspolys += mModel->numtris;

	Alias_SetupFrame(mModel,eEntity,&lLerpData);
	Alias_SetupEntityTransform(eEntity, &lLerpData);

	Video_ResetCapabilities(false);

	glPushMatrix();

	VideoShader_Enable();

	if(r_drawflat_cheatsafe)
		glShadeModel(GL_FLAT);

	R_RotateForEntity(eEntity->origin,eEntity->angles);

	Alias_SetupLighting(eEntity);
	Alias_DrawFrame(mModel, eEntity, lLerpData);

	if(r_drawflat_cheatsafe)
	{
		// Restore randomness
		srand((int)(cl.time*1000));

		glShadeModel(GL_SMOOTH);
	}

	VideoShader_Disable();

	glPopMatrix();

	Video_ResetCapabilities(true);

	// Show active light reference.
	if (dlLightSource)
		Draw_Line(eEntity->origin, dlLightSource->origin);
}
