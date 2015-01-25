/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "KatAlias.h"

/*
	Model Rendering Crap!
*/

#include "engine_videoshadow.h"
#include "engine_video.h"

extern cvar_t r_drawflat, gl_fullbrights, r_lerpmodels, r_lerpmove; //johnfitz

#define NUMVERTEXNORMALS	162

MathVector3_t	r_avertexnormals[NUMVERTEXNORMALS] =
{
	{	-0.525731,	0.000000,	0.850651	},
	{	-0.442863,	0.238856,	0.864188	},
	{	-0.295242,	0.000000,	0.955423	},
	{	-0.309017,	0.500000,	0.809017	},
	{	-0.162460,	0.262866,	0.951056	},
	{	0.000000,	0.000000,	1.000000	},
	{	0.000000,	0.850651,	0.525731	},
	{	-0.147621,	0.716567,	0.681718	},
	{	0.147621,	0.716567,	0.681718	},
	{	0.000000,	0.525731,	0.850651	},
	{	0.309017,	0.500000,	0.809017	},
	{	0.525731,	0.000000,	0.850651	},
	{	0.295242,	0.000000,	0.955423	},
	{	0.442863,	0.238856,	0.864188	},
	{	0.162460,	0.262866,	0.951056	},
	{	-0.681718,	0.147621,	0.716567	},
	{	-0.809017,	0.309017,	0.500000	},
	{	-0.587785,	0.425325,	0.688191	},
	{	-0.850651,	0.525731,	0.000000	},
	{	-0.864188,	0.442863,	0.238856	},
	{	-0.716567,	0.681718,	0.147621	},
	{   -0.688191,  0.587785,   0.425325    },
	{	-0.500000,	0.809017,	0.309017	},
	{	-0.238856,	0.864188,	0.442863	},
	{	-0.425325,	0.688191,	0.587785	},
	{	-0.716567,	0.681718,	-0.147621	},
	{	-0.500000,	0.809017,	-0.309017	},
	{-0.525731, 0.850651, 0.000000},
	{0.000000, 0.850651, -0.525731},
	{-0.238856, 0.864188, -0.442863},
	{0.000000, 0.955423, -0.295242},
	{-0.262866, 0.951056, -0.162460},
	{0.000000, 1.000000, 0.000000},
	{0.000000, 0.955423, 0.295242},
	{-0.262866, 0.951056, 0.162460},
	{0.238856, 0.864188, 0.442863},
	{0.262866, 0.951056, 0.162460},
	{0.500000, 0.809017, 0.309017},
	{0.238856, 0.864188, -0.442863},
	{0.262866, 0.951056, -0.162460},
	{0.500000, 0.809017, -0.309017},
	{0.850651, 0.525731, 0.000000},
	{0.716567, 0.681718, 0.147621},
	{0.716567, 0.681718, -0.147621},
	{0.525731, 0.850651, 0.000000},
	{0.425325, 0.688191, 0.587785},
	{0.864188, 0.442863, 0.238856},
	{0.688191, 0.587785, 0.425325},
	{0.809017, 0.309017, 0.500000},
	{0.681718, 0.147621, 0.716567},
	{0.587785, 0.425325, 0.688191},
	{0.955423, 0.295242, 0.000000},
	{1.000000, 0.000000, 0.000000},
	{0.951056, 0.162460, 0.262866},
	{0.850651, -0.525731, 0.000000},
	{0.955423, -0.295242, 0.000000},
	{0.864188, -0.442863, 0.238856},
	{0.951056, -0.162460, 0.262866},
	{0.809017, -0.309017, 0.500000},
	{0.681718, -0.147621, 0.716567},
	{0.850651, 0.000000, 0.525731},
	{0.864188, 0.442863, -0.238856},
	{0.809017, 0.309017, -0.500000},
	{0.951056, 0.162460, -0.262866},
	{0.525731, 0.000000, -0.850651},
	{0.681718, 0.147621, -0.716567},
	{0.681718, -0.147621, -0.716567},
	{0.850651, 0.000000, -0.525731},
	{0.809017, -0.309017, -0.500000},
	{0.864188, -0.442863, -0.238856},
	{0.951056, -0.162460, -0.262866},
	{0.147621, 0.716567, -0.681718},
	{0.309017, 0.500000, -0.809017},
	{0.425325, 0.688191, -0.587785},
	{0.442863, 0.238856, -0.864188},
	{0.587785, 0.425325, -0.688191},
	{0.688191, 0.587785, -0.425325},
	{-0.147621, 0.716567, -0.681718},
	{-0.309017, 0.500000, -0.809017},
	{0.000000, 0.525731, -0.850651},
	{-0.525731, 0.000000, -0.850651},
	{-0.442863, 0.238856, -0.864188},
	{-0.295242, 0.000000, -0.955423},
	{-0.162460, 0.262866, -0.951056},
	{0.000000, 0.000000, -1.000000},
	{0.295242, 0.000000, -0.955423},
	{0.162460, 0.262866, -0.951056},
	{-0.442863, -0.238856, -0.864188},
	{-0.309017, -0.500000, -0.809017},
	{-0.162460, -0.262866, -0.951056},
	{0.000000, -0.850651, -0.525731},
	{-0.147621, -0.716567, -0.681718},
	{	0.147621,	-0.716567,	-0.681718	},
	{	0.000000,	-0.525731,	-0.850651	},
	{	0.309017,	-0.500000,	-0.809017	},
	{	0.442863,	-0.238856,	-0.864188	},
	{	0.162460,	-0.262866,	-0.951056	},
	{	0.238856,	-0.864188,	-0.442863	},
	{	0.500000,	-0.809017,	-0.309017	},
	{	0.425325,	-0.688191,	-0.587785	},
	{	0.716567,	-0.681718,	-0.147621	},
	{	0.688191,	-0.587785,	-0.425325	},
	{	0.587785,	-0.425325,	-0.688191	},
	{0.000000, -0.955423, -0.295242},
	{0.000000, -1.000000, 0.000000},
	{0.262866, -0.951056, -0.162460},
	{0.000000, -0.850651, 0.525731},
	{0.000000, -0.955423, 0.295242},
	{0.238856, -0.864188, 0.442863},
	{	0.262866,	-0.951056,	0.162460	},
	{	0.500000,	-0.809017,	0.309017	},
	{	0.716567,	-0.681718,	0.147621	},
	{0.525731, -0.850651, 0.000000},
	{-0.238856, -0.864188, -0.442863},
	{-0.500000, -0.809017, -0.309017},
	{-0.262866, -0.951056, -0.162460},
	{-0.850651, -0.525731, 0.000000},
	{-0.716567, -0.681718, -0.147621},
	{-0.716567, -0.681718, 0.147621},
	{-0.525731, -0.850651, 0.000000},
	{-0.500000, -0.809017, 0.309017},
	{-0.238856, -0.864188, 0.442863},
	{-0.262866, -0.951056, 0.162460},
	{-0.864188, -0.442863, 0.238856},
	{-0.809017, -0.309017, 0.500000},
	{-0.688191, -0.587785, 0.425325},
	{-0.681718, -0.147621, 0.716567},
	{-0.442863, -0.238856, 0.864188},
	{-0.587785, -0.425325, 0.688191},
	{-0.309017, -0.500000, 0.809017},
	{-0.147621, -0.716567, 0.681718},
	{-0.425325, -0.688191, 0.587785},
	{-0.162460, -0.262866, 0.951056},
	{0.442863, -0.238856, 0.864188},
	{0.162460, -0.262866, 0.951056},
	{0.309017, -0.500000, 0.809017},
	{0.147621, -0.716567, 0.681718},
	{0.000000, -0.525731, 0.850651},
	{0.425325, -0.688191, 0.587785},
	{0.587785, -0.425325, 0.688191},
	{0.688191, -0.587785, 0.425325},
	{-0.955423, 0.295242, 0.000000},
	{-0.951056, 0.162460, 0.262866},
	{-1.000000, 0.000000, 0.000000},
	{-0.850651, 0.000000, 0.525731},
	{-0.955423, -0.295242, 0.000000},
	{-0.951056, -0.162460, 0.262866},
	{-0.864188, 0.442863, -0.238856},
	{-0.951056, 0.162460, -0.262866},
	{-0.809017, 0.309017, -0.500000},
	{-0.864188, -0.442863, -0.238856},
	{-0.951056, -0.162460, -0.262866},
	{-0.809017, -0.309017, -0.500000},
	{-0.681718, 0.147621, -0.716567},
	{-0.681718, -0.147621, -0.716567},
	{-0.850651, 0.000000, -0.525731},
	{	-0.688191,	0.587785,	-0.425325	},
	{   -0.587785,  0.425325,   -0.688191   },
	{	-0.425325,	0.688191,	-0.587785	},
	{	-0.425325,	-0.688191,	-0.587785	},
	{	-0.587785,	-0.425325,	-0.688191	},
	{	-0.688191,	-0.587785,	-0.425325	},
};

float	entalpha; //johnfitz

bool	bOverbright,
		bShading;		//johnfitz -- if false, disable vertex shading for various reasons (fullbright, r_lightmap, showtris, etc)

MathVector3_t	vLightColour;

DynamicLight_t	*dlLightSource;

void R_SetupModelLighting(vec3_t vOrigin)
{
	float			fDistance;

	if(!bShading)
		return;

	// Check to see if we can grab the light source, for directional information.
	dlLightSource = Light_GetDynamic(vOrigin);
	if(dlLightSource)
		Math_VectorCopy(dlLightSource->color,vLightColour);
	else
		Math_MVToVector(Light_GetSample(vOrigin), vLightColour);

	// Minimum light value on players (8)
	if(currententity > cl_entities && currententity <= cl_entities + cl.maxclients)
	{
		fDistance = 24.0f-(vLightColour[0]+vLightColour[1]+vLightColour[2]);
		if(fDistance > 0.0f)
			Math_VectorAddValue(vLightColour,fDistance/3.0f,vLightColour);
	}

	// [16/5/2013] BUG: Doesn't work since effects aren't being sent over... Poop ~hogsy
	if(currententity->effects & EF_FULLBRIGHT)
		Math_VectorSet(1.0f,vLightColour);

	Math_VectorScale(vLightColour,1.0f/200.0f,vLightColour);
}

void Alias_DrawFrame(MD2_t *mModel,entity_t *eEntity,lerpdata_t lLerpData)
{
#if 0	// Broken
	int					i,j,k,iVert;
	float               fAlpha;
	VideoObject_t		*voModel;
	MD2TriangleVertex_t	*mtvVertices,
						*mtvLerpVerts;
	MD2Triangle_t		*mtTriangles;
	MD2Frame_t			*mfFirst,*mfSecond;
	vec3_t				scale1,
                        scale2;

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

	voModel = (VideoObject_t*)Hunk_TempAlloc(mModel->num_glcmds*sizeof(VideoObject_t));

	mtTriangles	= (MD2Triangle_t*)((uint8_t*)mModel+mModel->ofs_tris);
	for(iVert = 0,i = 0; i < mModel->numtris; i++,mtTriangles++)
        for(k = 0; k < 3; k++)
        {
            for(j = 0; j < 3; j++)
            {
                voModel[iVert].vVertex[j] =	(mtvVertices[mtTriangles->index_xyz[k]].v[j]*scale1[j]+mfFirst->translate[j])*(1.0f-lLerpData.blend)+
											(mtvLerpVerts[mtTriangles->index_xyz[k]].v[j]*scale2[j]+mfSecond->translate[j])*lLerpData.blend;

                if(bShading)
					voModel[iVert].vColour[j] = (shadedots[mtvVertices[mtTriangles->index_xyz[k]].lightnormalindex])/2.0f;
				else
					// Otherwise give us a default colour of white.
					voModel[iVert].vColour[j] = 1.0f;
            }

			voModel[iVert].vTextureCoord[0][0] = (float)mModel->mtcTextureCoord[mtTriangles->index_st[k]].S / (float)mModel->skinwidth;
			voModel[iVert].vTextureCoord[0][1] = (float)mModel->mtcTextureCoord[mtTriangles->index_st[k]].T / (float)mModel->skinheight;

			voModel[iVert].vColour[3] = fAlpha;

			iVert++;
        }

	Video_DrawObject(voModel, VIDEO_PRIMITIVE_TRIANGLES, mModel->num_glcmds, Material_Get(eEntity->model->iAssignedMaterials), eEntity->skinnum);
#else
	float				ilerp;
	int					*order, count;
	MD2TriangleVertex_t	*verts1, *verts2;
	MD2Frame_t			*frame1, *frame2;
	Material_t			*mModelMat;
	VideoObject_t		*voModel;

	mModelMat = Material_Get(eEntity->model->iAssignedMaterials);

	ilerp = 1.0f - lLerpData.blend;

	//new version by muff - fixes bug, easier to read, faster (well slightly)
	frame1 = (MD2Frame_t*)((int)mModel + mModel->ofs_frames + (mModel->framesize*currententity->draw_lastpose));
	frame2 = (MD2Frame_t*)((int)mModel + mModel->ofs_frames + (mModel->framesize*currententity->draw_pose));

	if((currententity->scale != 1.0f) && (currententity->scale > 0.1f))
		glScalef(currententity->scale, currententity->scale, currententity->scale);

	verts1 = &frame1->verts[0];
	verts2 = &frame2->verts[0];

	order = (int*)((int)mModel + mModel->ofs_glcmds);

	voModel = (VideoObject_t*)Hunk_TempAlloc(4*sizeof(VideoObject_t));

	{
		unsigned int	uiVerts = 0;

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
				
				if (bShading)
				{
#if 0
					Video_ObjectColour(&voModel[uiVerts],
						(shadedots[verts1->lightnormalindex] * ilerp + shadedots[verts2->lightnormalindex] * lLerpData.blend),
						(shadedots[verts1->lightnormalindex] * ilerp + shadedots[verts2->lightnormalindex] * lLerpData.blend),
						(shadedots[verts1->lightnormalindex] * ilerp + shadedots[verts2->lightnormalindex] * lLerpData.blend),
						entalpha);
#endif
					Video_ObjectColour(&voModel[uiVerts],
						vLightColour[pRED],
						vLightColour[pGREEN],
						vLightColour[pBLUE],
						entalpha);
				}
				else
					Video_ObjectColour(&voModel[uiVerts], 1.0f, 1.0f, 1.0f, 1.0f);

				uiVerts++;

				order += 3;
			} while (--count);

			Video_DrawObject(voModel, VIDEO_PRIMITIVE_TRIANGLE_FAN, uiVerts, mModelMat, eEntity->skinnum);
		}
	}
#endif
}

void Alias_SetupFrame(MD2_t *mModel,lerpdata_t *ldLerp)
{
	if((currententity->frame >= mModel->num_frames) || (currententity->frame < 0))
	{
		Con_Warning("No such frame! (%d) (%s)\n",currententity->frame,currententity->model->name);

		currententity->frame = 0;
	}

	// [13/9/2012] Added check for r_lerpmodels to solve issue ~hogsy
	if(r_lerpmodels.value >= 1 && currententity->draw_lastmodel == currententity->model)
	{
		if(currententity->frame != currententity->draw_pose)
		{
			currententity->draw_lastpose	= currententity->draw_pose;
			currententity->draw_pose		= currententity->frame;
			currententity->draw_lerpstart	= cl.time;

			ldLerp->blend = 0;
		}
		// [13/9/2012] Removed check for r_lerpmodels here since it's now done above ~hogsy
		else
			ldLerp->blend = (cl.time-currententity->draw_lerpstart)*20.0;
	}
	else
	{
		currententity->draw_lastmodel	= currententity->model;
		currententity->draw_lastpose	= currententity->draw_pose = currententity->frame;
		currententity->draw_lerpstart	= cl.time;

		ldLerp->blend = 0;
	}

	if(ldLerp->blend > 1.0f)
		ldLerp->blend = 1.0f;
}

void Alias_SetupEntityTransform(lerpdata_t *lerpdata)
{
	float	blend;
	int		i;
	vec3_t	d;

	// if LERP_RESETMOVE, kill any lerps in progress
	if(currententity->lerpflags & LERP_RESETMOVE)
	{
		currententity->movelerpstart = 0;

		Math_VectorCopy(currententity->origin,currententity->previousorigin);
		Math_VectorCopy(currententity->origin,currententity->currentorigin);
		Math_VectorCopy(currententity->angles,currententity->previousangles);
		Math_VectorCopy(currententity->angles,currententity->currentangles);

		currententity->lerpflags -= LERP_RESETMOVE;
	}
	else if(!Math_VectorCompare(currententity->origin,currententity->currentorigin) || !Math_VectorCompare(currententity->angles,currententity->currentangles)) // origin/angles changed, start new lerp
	{
		currententity->movelerpstart = cl.time;

		Math_VectorCopy(currententity->currentorigin,currententity->previousorigin);
		Math_VectorCopy(currententity->origin,currententity->currentorigin);
		Math_VectorCopy(currententity->currentangles,currententity->previousangles);
		Math_VectorCopy(currententity->angles,currententity->currentangles);
	}

	//set up values
	if(r_lerpmove.value && currententity != &cl.viewent && currententity->lerpflags & LERP_MOVESTEP)
	{
		if(currententity->lerpflags & LERP_FINISH)
			blend = Math_Clamp(0, (cl.time - currententity->movelerpstart) / (currententity->lerpfinish - currententity->movelerpstart), 1);
		else
			blend = Math_Clamp(0, (cl.time - currententity->movelerpstart) / 0.1, 1);

		//translation
		Math_VectorSubtract(currententity->currentorigin,currententity->previousorigin,d);
		lerpdata->origin[0] = currententity->previousorigin[0]+d[0]*blend;
		lerpdata->origin[1] = currententity->previousorigin[1]+d[1]*blend;
		lerpdata->origin[2] = currententity->previousorigin[2]+d[2]*blend;

		//rotation
		Math_VectorSubtract(currententity->currentangles,currententity->previousangles,d);
		for(i = 0; i < 3; i++)
		{
			if (d[i] > 180.0f)
				d[i] -= 360.0f;
			if (d[i] < -180.0f)
				d[i] += 360.0f;
		}

		Math_VectorMA(currententity->previousangles,blend,d,lerpdata->angles);
	}
	else //don't lerp
	{
		Math_VectorCopy(currententity->origin,lerpdata->origin);
		Math_VectorCopy(currententity->angles,lerpdata->angles);
	}
}

// [23/8/2013] TODO: This seriously needs a damn header ~hogsy
void R_RotateForEntity(vec3_t origin,vec3_t angles);

/*	Draw the alias model.
*/
void Alias_Draw(entity_t *eEntity)
{
	lerpdata_t	lLerpData;
	MD2_t		*mModel;

	// [17/10/2013] Oops! Added this back in :) ~hogsy
	if(!cvVideoDrawModels.value || R_CullModelForEntity(eEntity))
		return;

	// [27/6/2013] Set defaults ~hogsy
	bShading	= true;
	entalpha    = ENTALPHA_DECODE(eEntity->alpha);

	mModel = (MD2_t*)Mod_Extradata(eEntity->model);

	// [23/8/2013] Update alias poly count! ~hogsy
	rs_aliaspolys += mModel->numtris;

	Alias_SetupFrame(mModel,&lLerpData);
	Alias_SetupEntityTransform(&lLerpData);

	glPushMatrix();

	if(r_drawflat_cheatsafe)
		glShadeModel(GL_FLAT);

	R_RotateForEntity(eEntity->origin,eEntity->angles);
	R_SetupModelLighting(eEntity->origin);

    Video_ResetCapabilities(false);

	// Let lighting stand out more...
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

	Alias_DrawFrame(mModel, eEntity, lLerpData);

	if(r_drawflat_cheatsafe)
	{
		// Restore randomness
		srand((int)(cl.time*1000));

		glShadeModel(GL_SMOOTH);
	}

	glPopMatrix();

    Video_ResetCapabilities(true);
}
