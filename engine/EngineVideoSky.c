/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2016 OldTimes Software

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

#define	MAX_CLIP_VERTS 64

extern	model_t	*loadmodel;
extern	int	rs_skypolys; //for r_speeds readout
extern	int rs_skypasses; //for r_speeds readout
float	skyflatcolor[3];
float	skymins[2][6], skymaxs[2][6];

char	cSkyBoxName[32];

gltexture_t	*gSkyBoxTexture[6],
			*gCloudTexture;

extern ConsoleVariable_t gl_farclip;
ConsoleVariable_t	r_fastsky = { "r_fastsky", "0" },
					r_sky_quality = { "r_sky_quality", "8" },
					r_skyalpha = { "r_skyalpha", "1" },
					r_skyfog = { "r_skyfog", "0.5" },
					cv_video_drawclouds = { "sky_drawclouds", "1", true, false, "Toggles rendering of clouds." },
					cv_sky_scrollspeed = { "sky_scrollspeed", "2.0", false, false, "Changes the speed at which the clouds scroll." };

int	skytexorder[6] = {0,2,1,3,4,5}; //for skybox

MathVector3f_t vSkyClip[6] =
{
	{	1,	1,	0	},
	{	1,	-1,	0	},
	{	0,	-1,	1	},
	{	0,	1,	1	},
	{	1,	0,	1	},
	{	-1,	0,	1	}
};

int	st_to_vec[6][3] =
{
	{	3,	-1,	2	},
	{	-3,	1,	2	},
	{	1,	3,	2	},
	{	-1,	-3,	2	},
	{	-2,	-1,	3	},	// straight up
	{	2,	-1,	-3	}	// straight down
};

int	vec_to_st[6][3] =
{
	{	-2,	3,	1	},
	{	2,	3,	-1	},
	{	1,	3,	2	},
	{	-1,	3,	-2	},
	{	-2,	-1,	3	},
	{	-2,	1,	-3	}
};

void Sky_LoadCloudTexture(const char *cPath)
{
	char				cFileName[PLATFORM_MAX_PATH];
	unsigned int		iWidth,iHeight;
	uint8_t				*bData;

	if(!cPath[0])
		return;

	sprintf(cFileName, "%ssky/%scloud", g_state.path_textures, cPath);

	bData = Image_LoadImage(cFileName,&iWidth,&iHeight);
	if(bData)
		gCloudTexture = TexMgr_LoadImage(
			cl.worldmodel,
			cFileName,
			iWidth,iHeight,
			SRC_RGBA,
			bData,
			cFileName,
			0,
			TEXPREF_ALPHA);
	else
	{
		Con_Warning("Failed to load %s!\n",cFileName);
		return;
	}
}

char *suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};

void Sky_LoadSkyBox (char *name)
{
	int		i, mark;
	unsigned int width, height;
	char	filename[PLATFORM_MAX_PATH];
	bool	bNoneFound = true;
	uint8_t	*data;

	if(strcmp(cSkyBoxName, name) == 0)
		return; //no change

	// Purge old textures
	for(i = 0; i < 6; i++)
	{
		if(gSkyBoxTexture[i] && gSkyBoxTexture[i] != notexture)
			TexMgr_FreeTexture(gSkyBoxTexture[i]);

		gSkyBoxTexture[i] = NULL;
	}

	// Turn off skybox if sky is set to ""
	if(name[0] == 0)
	{
		cSkyBoxName[0] = 0;
		return;
	}

	// Load textures
	for (i=0; i<6; i++)
	{
		mark = Hunk_LowMark ();

		sprintf(filename, "%ssky/%s%s", g_state.path_textures, name, suf[i]);
		data = Image_LoadImage (filename, &width, &height);
		if (data)
		{
			gSkyBoxTexture[i] = TexMgr_LoadImage (cl.worldmodel, filename, width, height, SRC_RGBA, data, filename, 0, TEXPREF_NONE);
			bNoneFound = false;
		}
		else
		{
			Con_Warning("Couldn't load %s\n", filename);
			gSkyBoxTexture[i] = notexture;
		}

		Hunk_FreeToLowMark (mark);
	}

	if(bNoneFound) // go back to scrolling sky if skybox is totally missing
	{
		for(i = 0; i < 6; i++)
		{
			if(gSkyBoxTexture[i] && gSkyBoxTexture[i] != notexture)
				TexMgr_FreeTexture(gSkyBoxTexture[i]);

			gSkyBoxTexture[i] = NULL;
		}
		cSkyBoxName[0] = 0;
		return;
	}

	strcpy(cSkyBoxName, name);
}

void Sky_NewMap (void)
{
	char	key[128],value[4096],
			*data;
	int		i;

	// Initially no sky
	cSkyBoxName[0] = 0;
	for(i=0; i<6; i++)
		gSkyBoxTexture[i] = NULL;

	gCloudTexture = NULL;

	// read worldspawn (this is so ugly, and shouldn't it be done on the server?)
	data = cl.worldmodel->entities;
	if (!data)
		return; //FIXME: how could this possibly ever happen? -- if there's no
	// worldspawn then the sever wouldn't send the loadmap message to the client

	data = COM_Parse(data);
	if (!data) //should never happen
		return; // error
	else if(com_token[0] != '{') //should never happen
		return; // error

	for(;;)
	{
		data = COM_Parse(data);
		if (!data)
			return; // error
		if (com_token[0] == '}')
			break; // end of worldspawn
		if (com_token[0] == '_')
			strcpy(key, com_token + 1);
		else
			strcpy(key, com_token);

		while (key[strlen(key)-1] == ' ') // remove trailing spaces
			key[strlen(key)-1] = 0;

		data = COM_Parse(data);
		if (!data)
			return; // error
		strcpy(value, com_token);

		if(!strcmp("sky",key))
			Sky_LoadSkyBox(value);

		if(!strcmp("cloud",key))
			Sky_LoadCloudTexture(value);

		if(!strcmp("scrollspeed",key))
			Cvar_SetValue(cv_sky_scrollspeed.name, (float)(atoi(value) / 10));
	}
}

void Sky_SkyCommand_f (void)
{
	switch (Cmd_Argc())
	{
	case 1:
		Con_Printf("\"sky\" is \"%s\"\n", cSkyBoxName);
		break;
	case 2:
		Sky_LoadSkyBox(Cmd_Argv(1));
		break;
	default:
		Con_Printf("usage: sky <skyname>\n");
	}
}

void Sky_Init(void)
{
	int		i;

	Cvar_RegisterVariable (&r_fastsky, NULL);
	Cvar_RegisterVariable (&r_sky_quality, NULL);
	Cvar_RegisterVariable (&r_skyalpha, NULL);
	Cvar_RegisterVariable (&r_skyfog, NULL);
	Cvar_RegisterVariable(&cv_video_drawclouds, NULL);
	Cvar_RegisterVariable(&cv_sky_scrollspeed, NULL);

	Cmd_AddCommand("sky", Sky_SkyCommand_f);

	for(i = 0; i < 6; i++)
		gSkyBoxTexture[i] = NULL;
}

//==============================================================================
//
//  PROCESS SKY SURFS
//
//==============================================================================

/*	Update sky bounds
*/
void Sky_ProjectPoly (int nump, MathVector3f_t vecs)
{
	int				i,j;
	MathVector3f_t	v, av;
	float			s, t, dv;
	int				axis;
	float			*vp;

	// decide which face it maps to
	Math_VectorCopy(pl_origin3f, v);
	for (i=0, vp=vecs ; i<nump ; i++, vp+=3)
		Math_VectorAdd (vp, v, v);

	av[0] = fabs(v[0]);
	av[1] = fabs(v[1]);
	av[2] = fabs(v[2]);
	if (av[0] > av[1] && av[0] > av[2])
	{
		if (v[0] < 0)
			axis = 1;
		else
			axis = 0;
	}
	else if (av[1] > av[2] && av[1] > av[0])
	{
		if (v[1] < 0)
			axis = 3;
		else
			axis = 2;
	}
	else
	{
		if (v[2] < 0)
			axis = 5;
		else
			axis = 4;
	}

	// project new texture coords
	for (i=0 ; i<nump ; i++, vecs+=3)
	{
		j = vec_to_st[axis][2];
		if (j > 0)
			dv = vecs[j - 1];
		else
			dv = -vecs[-j - 1];

		j = vec_to_st[axis][0];
		if (j < 0)
			s = -vecs[-j -1] / dv;
		else
			s = vecs[j-1] / dv;
		j = vec_to_st[axis][1];
		if (j < 0)
			t = -vecs[-j -1] / dv;
		else
			t = vecs[j-1] / dv;

		if (s < skymins[0][axis])
			skymins[0][axis] = s;
		if (t < skymins[1][axis])
			skymins[1][axis] = t;
		if (s > skymaxs[0][axis])
			skymaxs[0][axis] = s;
		if (t > skymaxs[1][axis])
			skymaxs[1][axis] = t;
	}
}

void Sky_ClipPoly (int nump, MathVector3f_t vecs, int stage)
{
	bool			bFront	= false,
					bBack	= false;
	float			*norm,*v,
					d,e,dists[MAX_CLIP_VERTS];
	int				sides[MAX_CLIP_VERTS],
					newc[2],
					i,j;
	MathVector3f_t	vNew[2][MAX_CLIP_VERTS];

	if(nump > MAX_CLIP_VERTS-2)
		Sys_Error ("Sky_ClipPoly: MAX_CLIP_VERTS");
	else if(stage == 6) // fully clipped
	{
		Sky_ProjectPoly (nump, vecs);
		return;
	}

	norm = vSkyClip[stage];
	for (i=0, v = vecs ; i<nump ; i++, v+=3)
	{
		d = Math_DotProduct (v, norm);
		if (d > pMath_EPSILON_ON)
		{
			bFront = true;

			sides[i] = SIDE_FRONT;
		}
		else if (d < pMath_EPSILON_ON)
		{
			bBack = true;

			sides[i] = SIDE_BACK;
		}
		else
			sides[i] = SIDE_ON;
		dists[i] = d;
	}

	if(!bFront || !bBack)
	{
		// not clipped
		Sky_ClipPoly (nump, vecs, stage+1);
		return;
	}

	// clip it
	sides[i] = sides[0];
	dists[i] = dists[0];
	Math_VectorCopy (vecs, (vecs+(i*3)) );
	newc[0] = newc[1] = 0;

	for (i=0, v = vecs ; i<nump ; i++, v+=3)
	{
		switch (sides[i])
		{
		case SIDE_FRONT:
			Math_VectorCopy(v,vNew[0][newc[0]]);
			newc[0]++;
			break;
		case SIDE_BACK:
			Math_VectorCopy(v,vNew[1][newc[1]]);
			newc[1]++;
			break;
		case SIDE_ON:
			Math_VectorCopy(v,vNew[0][newc[0]]);
			newc[0]++;
			Math_VectorCopy(v,vNew[1][newc[1]]);
			newc[1]++;
			break;
		}

		if (sides[i] == SIDE_ON || sides[i+1] == SIDE_ON || sides[i+1] == sides[i])
			continue;

		d = dists[i]/(dists[i]-dists[i+1]);
		for (j=0 ; j<3 ; j++)
		{
			e = v[j] + d*(v[j+3] - v[j]);
			vNew[0][newc[0]][j] = e;
			vNew[1][newc[1]][j] = e;
		}
		newc[0]++;
		newc[1]++;
	}

	// Continue
	Sky_ClipPoly(newc[0],vNew[0][0],stage+1);
	Sky_ClipPoly(newc[1],vNew[1][0],stage+1);
}

void Sky_ProcessPoly (glpoly_t	*p)
{
	int				i;
	MathVector3f_t	verts[MAX_CLIP_VERTS];

	// Draw it
	DrawGLPoly(p);

	rs_brushpasses++;

	// Update sky bounds
	if (!r_fastsky.value)
	{
		for (i=0 ; i<p->numverts ; i++)
			Math_VectorSubtract (p->verts[i], r_origin, verts[i]);
		Sky_ClipPoly (p->numverts, verts[0], 0);
	}
}

void Sky_ProcessTextureChains(void)
{
	unsigned int	i;
	msurface_t		*s;
	texture_t		*tTexture;

	if(!r_drawworld_cheatsafe)
		return;

	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		tTexture = cl.worldmodel->textures[i];
		if (!tTexture || !tTexture->texturechain || !(tTexture->texturechain->flags & SURF_DRAWSKY))
			continue;

		for(s = tTexture->texturechain; s; s = s->texturechain)
			if (!s->culled)
				Sky_ProcessPoly(s->polys);
	}
}

void Sky_ProcessEntities(void)
{
	int				k,mark;
	unsigned int	j;
	float			dot;
	bool			bRotated;
	ClientEntity_t	*e;
	msurface_t		*s;
	glpoly_t		*p;
	MathVector3f_t	vTemp, forward, right, up;

	if (!r_drawentities.value)
		return;

	for (unsigned int i=0 ; i<cl_numvisedicts ; i++)
	{
		e = cl_visedicts[i];

		if (e->model->type != MODEL_TYPE_LEVEL)
			continue;

		if(R_CullModelForEntity(e))
			continue;

		if(e->alpha == ENTALPHA_ZERO)
			continue;

		Math_VectorSubtract (r_refdef.vieworg, e->origin, modelorg);
		if(e->angles[0] || e->angles[1] || e->angles[2])
		{
			bRotated = true;

			plAngleVectors(e->angles, forward, right, up);
			Math_VectorCopy(modelorg,vTemp);

			modelorg[0] = Math_DotProduct(vTemp,forward);
			modelorg[1] = -Math_DotProduct(vTemp,right);
			modelorg[2] = Math_DotProduct(vTemp,up);
		}
		else
			bRotated = false;

		s = &e->model->surfaces[e->model->firstmodelsurface];

		for (j=0 ; j<e->model->nummodelsurfaces ; j++, s++)
		{
			if (s->flags & SURF_DRAWSKY)
			{
				dot = Math_DotProduct (modelorg, s->plane->normal) - s->plane->dist;
				if (((s->flags & SURF_PLANEBACK) && (dot < -BACKFACE_EPSILON)) ||
					(!(s->flags & SURF_PLANEBACK) && (dot > BACKFACE_EPSILON)))
				{
					//copy the polygon and translate manually, since Sky_ProcessPoly needs it to be in world space
					mark = Hunk_LowMark();
					p = (glpoly_t*)Hunk_Alloc (sizeof(*s->polys)); //FIXME: don't allocate for each poly
					p->numverts = s->polys->numverts;
					for (k=0; k<p->numverts; k++)
					{
						if(bRotated)
						{
							p->verts[k][0] = e->origin[0] + s->polys->verts[k][0] * forward[0]
														  - s->polys->verts[k][1] * right[0]
														  + s->polys->verts[k][2] * up[0];
							p->verts[k][1] = e->origin[1] + s->polys->verts[k][0] * forward[1]
														  - s->polys->verts[k][1] * right[1]
														  + s->polys->verts[k][2] * up[1];
							p->verts[k][2] = e->origin[2] + s->polys->verts[k][0] * forward[2]
														  - s->polys->verts[k][1] * right[2]
														  + s->polys->verts[k][2] * up[2];
						}
						else
							Math_VectorAdd(s->polys->verts[k], e->origin, p->verts[k]);
					}
					Sky_ProcessPoly (p);
					Hunk_FreeToLowMark (mark);
				}
			}
		}
	}
}

//==============================================================================
//
//  RENDER SKYBOX
//
//==============================================================================

void Sky_EmitSkyBoxVertex(float s,float t,int axis)
{
#ifdef KATANA_CORE_GL
	int				j,k;
	float			w,h;
	MathVector3f_t	v,b;

	b[0] = s*10.0f;
	b[1] = t*10.0f;
	b[2] = 10.0f;

	for (j=0 ; j<3 ; j++)
	{
		k = st_to_vec[axis][j];
		if (k < 0)
			v[j] = -b[-k - 1];
		else
			v[j] = b[k - 1];
		v[j] += r_origin[j];
	}

	// convert from range [-1,1] to [0,1]
	s = (s+1)*0.5f;
	t = (t+1)*0.5f;

	// Avoid bilerp seam
	w = gSkyBoxTexture[skytexorder[axis]]->width;
	h = gSkyBoxTexture[skytexorder[axis]]->height;
	s = s * (w-1)/w + 0.5/w;
	t = t * (h-1)/h + 0.5/h;

	t = 1.0f-t;
	glTexCoord2f(s,t);
	glVertex3fv(v);
#endif
}

void Sky_DrawSkyBox (void)
{
#ifdef KATANA_CORE_GL
	int	i;

	for(i = 0; i < 6; i++)
	{
		if (skymins[0][i] >= skymaxs[0][i] || skymins[1][i] >= skymaxs[1][i])
			continue;

		Video_SetTexture(gSkyBoxTexture[skytexorder[i]]);

#if 1 //FIXME: this is to avoid tjunctions until i can do it the right way
		skymins[0][i] = -1;
		skymins[1][i] = -1;
		skymaxs[0][i] = 1;
		skymaxs[1][i] = 1;
#endif

		glBegin(GL_QUADS);
		Sky_EmitSkyBoxVertex (skymins[0][i], skymins[1][i], i);
		Sky_EmitSkyBoxVertex (skymins[0][i], skymaxs[1][i], i);
		Sky_EmitSkyBoxVertex (skymaxs[0][i], skymaxs[1][i], i);
		Sky_EmitSkyBoxVertex (skymaxs[0][i], skymins[1][i], i);
		glEnd();

		rs_skypolys++;
		rs_skypasses++;

		if (Fog_GetDensity() > 0 && r_skyfog.value > 0)
		{
			float *c;

			c = Fog_GetColor();

			vlEnable(VIDEO_BLEND);
			vlDisable(VIDEO_TEXTURE_2D);

			glColor4f(c[0], c[1], c[2], Math_Clamp(0, r_skyfog.value, 1.0f));

			glBegin(GL_QUADS);
			Sky_EmitSkyBoxVertex (skymins[0][i], skymins[1][i], i);
			Sky_EmitSkyBoxVertex (skymins[0][i], skymaxs[1][i], i);
			Sky_EmitSkyBoxVertex (skymaxs[0][i], skymaxs[1][i], i);
			Sky_EmitSkyBoxVertex (skymaxs[0][i], skymins[1][i], i);
			glEnd();

			glColor3f(1.0f,1.0f,1.0f);

			vlEnable(VIDEO_TEXTURE_2D);
			vlDisable(VIDEO_BLEND);

			rs_skypasses++;
		}
	}
#endif
}

//==============================================================================
//
//  RENDER CLOUDS
//
//==============================================================================

void Sky_SetBoxVert (float s, float t, int axis, MathVector3f_t v)
{
	MathVector3f_t	b;
	int				j, k;

	b[0] = s * 10.0f;
	b[1] = t * 10.0f;
	b[2] = 10.0f;

	for (j=0 ; j<3 ; j++)
	{
		k = st_to_vec[axis][j];
		if (k < 0)
			v[j] = -b[-k - 1];
		else
			v[j] = b[k - 1];
		v[j] += r_origin[j];
	}
}

void Sky_GetTexCoord(MathVector3f_t v,float speed,float *s,float *t)
{
	MathVector3f_t	vDirection;
	float			fLength,fScroll;

	Math_VectorSubtract(v,r_origin,vDirection);

	vDirection[2] *= 3.0f;	// Flatten the sphere

	fLength = vDirection[0]*vDirection[0]+vDirection[1]*vDirection[1]+vDirection[2]*vDirection[2];
	fLength = sqrt(fLength);
	fLength = 6.0f*63.0f/fLength;

	fScroll = cl.time*speed;
	fScroll -= (int)fScroll & ~127;

	*s = (fScroll+vDirection[0]*fLength)*(1.0f/128.0f);
	*t = (fScroll+vDirection[1]*fLength)*(1.0f/128.0f);
}

void Sky_DrawFaceQuad(glpoly_t *p)
{
#if defined (VL_MODE_OPENGL)
	float	s,t,
			*v;
	int		i;

	glColor3f(1.0f,1.0f,1.0f);

	Video_SetTexture(gCloudTexture);

	vlEnable(VIDEO_BLEND);
	vlBlendFunc(VL_BLEND_ONE, VL_BLEND_ONE);

	glBegin(GL_QUADS);

	for (i = 0, v = p->verts[0]; i < 4; i++, v += VERTEXSIZE)
	{
		Sky_GetTexCoord(v,cv_sky_scrollspeed.value,&s,&t);

		glTexCoord2f(s,t);
		glVertex3fv(v);
	}

	glEnd();

	vlBlendFunc(VL_BLEND_DEFAULT);

	rs_skypolys++;
	rs_skypasses++;

	if(Fog_GetDensity() > 0 && r_skyfog.value > 0)
	{
		float *c = Fog_GetColor();

		vlDisable(VIDEO_TEXTURE_2D);

		glColor4f(c[0], c[1], c[2], Math_Clamp(0.0, r_skyfog.value, 1.0));

		glBegin(GL_QUADS);
		for(i = 0,v = p->verts[0]; i < 4; i++,v += VERTEXSIZE)
			glVertex3fv(v);
		glEnd();

		glColor3f(1.0f,1.0f,1.0f);

		vlEnable(VIDEO_TEXTURE_2D);

		rs_skypasses++;
	}

	vlDisable(VIDEO_BLEND);
#endif
}

void Sky_DrawFace (int axis)
{
	glpoly_t	*p;
	MathVector3f_t verts[4];
	int			i, j, start;
	float		di,qi,dj,qj;
	MathVector3f_t v_up, v_right, temp, temp2;

	Sky_SetBoxVert(-1.0f,-1.0f,axis,verts[0]);
	Sky_SetBoxVert(-1.0f,1.0f,axis,verts[1]);
	Sky_SetBoxVert(1.0f,1.0f,axis,verts[2]);
	Sky_SetBoxVert(1.0f,-1.0f,axis,verts[3]);

	start = Hunk_LowMark ();
	p = (glpoly_t*)Hunk_Alloc(sizeof(glpoly_t));

	Math_VectorSubtract(verts[2],verts[3], v_up);
	Math_VectorSubtract(verts[2],verts[1], v_right);

	di = Math_Max((int)r_sky_quality.value,1);
	qi = 1.0f/di;
	dj = (axis < 4) ? di*2 : di; // Subdivide vertically more than horizontally on skybox sides
	qj = 1.0f/dj;

	for (i=0; i<di; i++)
	{
		for (j=0; j<dj; j++)
		{
			if (i*qi < skymins[0][axis]/2+0.5 - qi || i*qi > skymaxs[0][axis]/2+0.5 ||
				j*qj < skymins[1][axis]/2+0.5 - qj || j*qj > skymaxs[1][axis]/2+0.5)
				continue;

			//if (i&1 ^ j&1) continue; //checkerboard test
			Math_VectorScale(v_right, qi*i, temp);
			Math_VectorScale(v_up, qj*j, temp2);
			Math_VectorAdd(temp,temp2,temp);
			Math_VectorAdd(verts[0],temp,p->verts[0]);

			Math_VectorScale(v_up, qj, temp);
			Math_VectorAdd(p->verts[0],temp,p->verts[1]);

			Math_VectorScale(v_right, qi, temp);
			Math_VectorAdd(p->verts[1],temp,p->verts[2]);

			Math_VectorAdd(p->verts[0],temp,p->verts[3]);

			Sky_DrawFaceQuad(p);
		}
	}

	Hunk_FreeToLowMark(start);
}

// Sky Camera

void R_SetupView(void);
void R_RenderScene(void);

bool			sky_camera;
MathVector3f_t	sky_camerapos;

void Sky_ReadCameraPosition(void)
{
	MathVector3f_t campos;

	sky_camera = MSG_ReadByte();
	if (!sky_camera)
		return;

	campos[0] = MSG_ReadCoord();
	campos[1] = MSG_ReadCoord();
	campos[2] = MSG_ReadCoord();
	Math_VectorCopy(campos, sky_camerapos);
}

/*	Hacky way of rendering a 3D skybox.
		- Consider rendering 3D skybox once into envmap?
		- Consider using FBO instead and somehow spanning it?
*/
void Sky_Draw3DWorld(void)
{
#ifdef KATANA_CORE_GL
	MathVector3f_t oldorg;

	// Don't let us render twice.
	if (!sky_camera || r_refdef.sky)
		return;

	// Copy the matrix over.
	memcpy(r_base_world_matrix, r_world_matrix, sizeof(r_base_world_matrix));

	// Update view position.
	plVectorCopy3fv(r_refdef.vieworg, oldorg);
	plVectorCopy3fv(sky_camerapos, r_refdef.vieworg);
	
	R_SetupView();
	R_SetupScene();

	glScalef(2.0f, 2.0f, 2.0f);
	glTranslatef(
		r_refdef.vieworg[0],
		r_refdef.vieworg[1],
		r_refdef.vieworg[2]
	);

	r_refdef.sky = true;
	World_Draw();
	r_refdef.sky = false;

	// Restore view position.
	plVectorCopy3fv(oldorg, r_refdef.vieworg);

	// Restore the original matrix.
	glLoadMatrixf(r_base_world_matrix);

	// Setup the view again, urgh.
	R_SetupView();
#endif
}

//

/*	Called once per frame before drawing anything else
*/
void Sky_Draw(void)
{
#ifdef KATANA_CORE_GL
	int	i;

	// In these special render modes, the sky faces are handled in the normal world/brush renderer
	if (r_drawflat_cheatsafe || r_lightmap_cheatsafe || !cv_video_drawsky.bValue)
		return;

	// Reset sky bounds.
	for(i = 0; i < 6; i++)
	{
		skymins[0][i] = skymins[1][i] = 9999;
		skymaxs[0][i] = skymaxs[1][i] = -9999;
	}

	// Process world and bmodels: draw flat-shaded sky surfs, and update skybounds
	Fog_DisableGFog();

	vlDisable(VIDEO_TEXTURE_2D);

	if(Fog_GetDensity() > 0)
		glColor3fv(Fog_GetColor());
	else
		glColor3fv(skyflatcolor);

	Sky_ProcessTextureChains();
	Sky_ProcessEntities();

	glColor3f(1.0f,1.0f,1.0f);

	vlEnable(VIDEO_TEXTURE_2D);

	// Render slow sky: cloud layers or skybox
	if(!r_fastsky.value && !(Fog_GetDensity() > 0 && r_skyfog.value >= 1))
	{
		vlDisable(VIDEO_DEPTH_TEST);

		// By default we use a skybox.
		if(cSkyBoxName[0])
			Sky_DrawSkyBox();

		if(cv_video_drawclouds.value && gCloudTexture)
		{
			// Draw the scrolling clouds...
			for(i = 0; i < 6; i++)
				if(skymins[0][i] < skymaxs[0][i] && skymins[1][i] < skymaxs[1][i])
					Sky_DrawFace(i);
		}

		vlEnable(VIDEO_DEPTH_TEST);
	}

	Fog_EnableGFog();

	// 3D skybox support.
	if (cl.worldmodel && (cl.worldmodel->flags & MODEL_FLAG_3DSKY))
		Sky_Draw3DWorld();
#endif
}
