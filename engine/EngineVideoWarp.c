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

#include <stdlib.h>

#include "engine_base.h"

/*
	Warping animation support.
*/

#include "video.h"

extern ConsoleVariable_t r_drawflat;

int gl_warpimagesize;

ConsoleVariable_t r_oldwater		= {	"r_oldwater",		"1",	true,	false	};
ConsoleVariable_t r_waterquality = { "r_waterquality", "8" };
ConsoleVariable_t r_waterwarp = { "r_waterwarp", "1" };

float turbsin[] =
{
	0, 0.19633, 0.392541, 0.588517, 0.784137, 0.979285, 1.17384, 1.3677,
	1.56072, 1.75281, 1.94384, 2.1337, 2.32228, 2.50945, 2.69512, 2.87916,
	3.06147, 3.24193, 3.42044, 3.59689, 3.77117, 3.94319, 4.11282, 4.27998,
	4.44456, 4.60647, 4.76559, 4.92185, 5.07515, 5.22538, 5.37247, 5.51632,
	5.65685, 5.79398, 5.92761, 6.05767, 6.18408, 6.30677, 6.42566, 6.54068,
	6.65176, 6.75883, 6.86183, 6.9607, 7.05537, 7.14579, 7.23191, 7.31368,
	7.39104, 7.46394, 7.53235, 7.59623, 7.65552, 7.71021, 7.76025, 7.80562,
	7.84628, 7.88222, 7.91341, 7.93984, 7.96148, 7.97832, 7.99036, 7.99759,
	8, 7.99759, 7.99036, 7.97832, 7.96148, 7.93984, 7.91341, 7.88222,
	7.84628, 7.80562, 7.76025, 7.71021, 7.65552, 7.59623, 7.53235, 7.46394,
	7.39104, 7.31368, 7.23191, 7.14579, 7.05537, 6.9607, 6.86183, 6.75883,
	6.65176, 6.54068, 6.42566, 6.30677, 6.18408, 6.05767, 5.92761, 5.79398,
	5.65685, 5.51632, 5.37247, 5.22538, 5.07515, 4.92185, 4.76559, 4.60647,
	4.44456, 4.27998, 4.11282, 3.94319, 3.77117, 3.59689, 3.42044, 3.24193,
	3.06147, 2.87916, 2.69512, 2.50945, 2.32228, 2.1337, 1.94384, 1.75281,
	1.56072, 1.3677, 1.17384, 0.979285, 0.784137, 0.588517, 0.392541, 0.19633,
	9.79717e-16, -0.19633, -0.392541, -0.588517, -0.784137, -0.979285, -1.17384, -1.3677,
	-1.56072, -1.75281, -1.94384, -2.1337, -2.32228, -2.50945, -2.69512, -2.87916,
	-3.06147, -3.24193, -3.42044, -3.59689, -3.77117, -3.94319, -4.11282, -4.27998,
	-4.44456, -4.60647, -4.76559, -4.92185, -5.07515, -5.22538, -5.37247, -5.51632,
	-5.65685, -5.79398, -5.92761, -6.05767, -6.18408, -6.30677, -6.42566, -6.54068,
	-6.65176, -6.75883, -6.86183, -6.9607, -7.05537, -7.14579, -7.23191, -7.31368,
	-7.39104, -7.46394, -7.53235, -7.59623, -7.65552, -7.71021, -7.76025, -7.80562,
	-7.84628, -7.88222, -7.91341, -7.93984, -7.96148, -7.97832, -7.99036, -7.99759,
	-8, -7.99759, -7.99036, -7.97832, -7.96148, -7.93984, -7.91341, -7.88222,
	-7.84628, -7.80562, -7.76025, -7.71021, -7.65552, -7.59623, -7.53235, -7.46394,
	-7.39104, -7.31368, -7.23191, -7.14579, -7.05537, -6.9607, -6.86183, -6.75883,
	-6.65176, -6.54068, -6.42566, -6.30677, -6.18408, -6.05767, -5.92761, -5.79398,
	-5.65685, -5.51632, -5.37247, -5.22538, -5.07515, -4.92185, -4.76559, -4.60647,
	-4.44456, -4.27998, -4.11282, -3.94319, -3.77117, -3.59689, -3.42044, -3.24193,
	-3.06147, -2.87916, -2.69512, -2.50945, -2.32228, -2.1337, -1.94384, -1.75281,
	-1.56072, -1.3677, -1.17384, -0.979285, -0.784137, -0.588517, -0.392541, -0.19633,
};

#define WARPCALC(s,t) ((s + turbsin[(int)((t*2)+(cl.time*(128.0/pMath_PI))) & 255]) * (1.0/64)) //johnfitz -- correct warp

//==============================================================================
//
//  OLD-STYLE WATER
//
//==============================================================================

extern	model_t	*loadmodel;

msurface_t	*warpface;

cvar_t gl_subdivide_size = {"gl_subdivide_size", "128", true, false, "Changes the number of subdevides used for water.\nCan result in better water quality while resulting in dropped performance." };

void BoundPoly(int numverts, float *verts, plVector3f_t mins, plVector3f_t maxs)
{
	int		i, j;
	float	*v;

	mins[0] = mins[1] = mins[2] = 9999;
	maxs[0] = maxs[1] = maxs[2] = -9999;
	v = verts;
	for (i=0 ; i<numverts ; i++)
		for (j=0 ; j<3 ; j++, v++)
		{
			if (*v < mins[j])
				mins[j] = *v;
			if (*v > maxs[j])
				maxs[j] = *v;
		}
}

void SubdividePolygon (int numverts, float *verts)
{
	int				i, j, k;
	plVector3f_t	mins, maxs;
	float			m;
	float			*v;
	plVector3f_t	front[64], back[64];
	int				f, b;
	float			dist[64] = { 0 };
	float			frac;
	glpoly_t		*poly;
	float			s, t;

	if(numverts > 60)
		Sys_Error ("numverts = %i", numverts);

	BoundPoly (numverts, verts, mins, maxs);

	for(i = 0; i < 3; i++)
	{
		m = (mins[i] + maxs[i]) * 0.5;
		m = gl_subdivide_size.value * floor (m/gl_subdivide_size.value + 0.5);
		if (maxs[i] - m < 8)
			continue;
		if (m - mins[i] < 8)
			continue;

		// cut it
		v = verts + i;
		for (j=0 ; j<numverts ; j++, v+= 3)
			dist[j] = *v - m;

		// wrap cases
		dist[j] = dist[0];
		v-=i;
		Math_VectorCopy (verts, v);

		f = b = 0;
		v = verts;
		for (j=0 ; j<numverts ; j++, v+= 3)
		{
			if (dist[j] >= 0)
			{
				Math_VectorCopy (v, front[f]);
				f++;
			}
			if (dist[j] <= 0)
			{
				Math_VectorCopy (v, back[b]);
				b++;
			}
			if (dist[j] == 0 || dist[j+1] == 0)
				continue;
			if ( (dist[j] > 0) != (dist[j+1] > 0) )
			{
				// clip point
				frac = dist[j] / (dist[j] - dist[j+1]);
				for (k=0 ; k<3 ; k++)
					front[f][k] = back[b][k] = v[k] + frac*(v[3+k] - v[k]);
				f++;
				b++;
			}
		}

		SubdividePolygon (f, front[0]);
		SubdividePolygon (b, back[0]);
		return;
	}

	poly = (glpoly_t*)Hunk_Alloc (sizeof(glpoly_t) + (numverts-4) * VERTEXSIZE*sizeof(float));
	poly->next = warpface->polys->next;
	warpface->polys->next = poly;
	poly->numverts = numverts;
	for (i=0 ; i<numverts ; i++, verts+= 3)
	{
		Math_VectorCopy (verts, poly->verts[i]);
		s = Math_DotProduct (verts, warpface->texinfo->vecs[0]);
		t = Math_DotProduct (verts, warpface->texinfo->vecs[1]);
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;
	}
}

void GL_SubdivideSurface(msurface_t *fa)
{
	plVector3f_t	verts[64];
	int				i;

	warpface = fa;

	//the first poly in the chain is the undivided poly for newwater rendering.
	//grab the verts from that.
	for (i=0; i<fa->polys->numverts; i++)
		Math_VectorCopy (fa->polys->verts[i], verts[i]);

	SubdividePolygon (fa->polys->numverts, verts[0]);
}

/*	TODO:
		- Firstly save each iteration of our water, so we can keep track of colour etc.
		- Recalc colour for different dynamic moving lights.
*/
void Surface_DrawWater(glpoly_t *p, Material_t *material)
{
	vlVertex_t *voWaterPoly = calloc(p->numverts, sizeof(vlVertex_t));
	if(!voWaterPoly)
	{
		Sys_Error("Failed to allocate water poly!\n");
		return;
	}

	float fWaterAlpha = Math_Clamp(0, material->fAlpha, 1.0f);

	float *vertex = p->verts[0];
	for (int i = 0; i < p->numverts; i++, vertex += VERTEXSIZE)
	{
		MathVector3f_t vWave;
		Math_VectorCopy(vertex, vWave);

		Video_ObjectTexture(&voWaterPoly[i], VIDEO_TEXTURE_DIFFUSE, vertex[3], vertex[4]);
		//Video_ObjectTexture(&voWaterPoly[i], VIDEO_TEXTURE_DIFFUSE, WARPCALC(fVert[3], fVert[4]), WARPCALC(fVert[4], fVert[3]));
		Video_ObjectTexture(&voWaterPoly[i], VIDEO_TEXTURE_LIGHT, vertex[5], vertex[6]);
		Video_ObjectColour(&voWaterPoly[i], 1.0f, 1.0f, 1.0f, fWaterAlpha);

		// Subtle water bobbing, based on this code http://www.quake-1.com/docs/quakesrc.org/26.html
		vWave[2] = vertex[2] +
			2.0f*(float)sin(vertex[0] * 0.025f + cl.time)*(float)sin(vertex[2] * 0.05f + cl.time) +
			2.0f*(float)sin(vertex[1] * 0.025f + cl.time*2.0f)*(float)sin(vertex[2] * 0.05f + cl.time);
		Video_ObjectVertex(&voWaterPoly[i], vWave[0], vWave[1], vWave[2]);
	}

	Video_DrawObject(voWaterPoly, VL_PRIMITIVE_TRIANGLE_FAN, p->numverts, material, 0);

	free(voWaterPoly);
}

//==============================================================================
//
//  RENDER-TO-FRAMEBUFFER WATER
//
//==============================================================================

/*	Each frame, update warping textures
*/
void R_UpdateWarpTextures (void)
{
#if 0
	texture_t *tx;
	int i;
	float x, y, x2, warptess;

	if (r_oldwater.value || cl.bIsPaused || r_drawflat_cheatsafe || r_lightmap_cheatsafe)
		return;

	warptess = 128.0f / Math_Clamp(3.0f, floor(r_waterquality.value), 64.0f);

	for (i=0; i<cl.worldmodel->numtextures; i++)
	{
		tx = cl.worldmodel->textures[i];
		if(!tx)
			continue;

		if(!tx->update_warp)
			continue;

		//render warp
		GL_SetCanvas (CANVAS_WARPIMAGE);
		Video_SetTexture(Material_Get(tx->iAssignedMaterial));
		for (x=0.0; x<128.0; x=x2)
		{
			x2 = x + warptess;
			glBegin (GL_TRIANGLE_STRIP);
			for (y=0.0; y<128.01; y+=warptess) // .01 for rounding errors
			{
				glTexCoord2f (WARPCALC(x,y), WARPCALC(y,x));
				glVertex2f (x,y);
				glTexCoord2f (WARPCALC(x2,y), WARPCALC(y,x2));
				glVertex2f (x2,y);
			}
			glEnd();
		}

		//copy to texture
		Video_SetTexture(tx->warpimage);
		glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, glx, gly+glheight-gl_warpimagesize, gl_warpimagesize, gl_warpimagesize);

		tx->update_warp = FALSE;
	}
#endif
}
