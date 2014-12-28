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
#include "quakedef.h"

/*
	World
*/

#include "engine_video.h"
#include "engine_videomaterial.h"

extern cvar_t gl_fullbrights, r_drawflat, gl_overbright, r_oldwater, r_oldskyleaf, r_showtris; //johnfitz

extern glpoly_t	*lightmap_polys[MAX_LIGHTMAPS];

byte *SV_FatPVS (vec3_t org, model_t *worldmodel);
extern byte mod_novis[BSP_MAX_LEAFS/8];

bool bVisibilityChanged; //if true, force pvs to be refreshed

//==============================================================================
//
// SETUP CHAINS
//
//==============================================================================

void R_MarkSurfaces (void)
{
	byte			*vis;
	mleaf_t			*leaf;
	mnode_t			*node;
	msurface_t		*surf, **mark;
	int				i;
	unsigned int	j;
	bool			nearwaterportal;

	// clear lightmap chains
	memset(lightmap_polys,0,sizeof(lightmap_polys));

	// check this leaf for water portals
	// TODO: loop through all water surfs and use distance to leaf cullbox
	nearwaterportal = false;
	for (i=0, mark = r_viewleaf->firstmarksurface; i < r_viewleaf->nummarksurfaces; i++, mark++)
		if ((*mark)->flags & SURF_DRAWTURB)
			nearwaterportal = true;

	// Choose vis data
	if (r_novis.value || r_viewleaf->contents == BSP_CONTENTS_SOLID || r_viewleaf->contents == BSP_CONTENTS_SKY)
		vis = &mod_novis[0];
	else if(nearwaterportal)
		vis = SV_FatPVS (r_origin, cl.worldmodel);
	else
		vis = Mod_LeafPVS (r_viewleaf, cl.worldmodel);

	// if surface chains don't need regenerating, just add static entities and return
	if (r_oldviewleaf == r_viewleaf && !bVisibilityChanged && !nearwaterportal)
	{
		leaf = &cl.worldmodel->leafs[1];
		for (i=0 ; i<cl.worldmodel->numleafs ; i++, leaf++)
			if (vis[i>>3] & (1<<(i&7)))
				if (leaf->efrags)
					R_StoreEfrags (&leaf->efrags);
		return;
	}

	r_visframecount++;
	r_oldviewleaf = r_viewleaf;

	// iterate through leaves, marking surfaces
	leaf = &cl.worldmodel->leafs[1];
	for (i=0 ; i<cl.worldmodel->numleafs ; i++, leaf++)
	{
		if (vis[i>>3] & (1<<(i&7)))
		{
			if (r_oldskyleaf.value || leaf->contents != BSP_CONTENTS_SKY)
				for (j=0, mark = leaf->firstmarksurface; j<leaf->nummarksurfaces; j++, mark++)
					(*mark)->visframe = r_visframecount;

			// add static models
			if (leaf->efrags)
				R_StoreEfrags (&leaf->efrags);
		}
	}

	// set all chains to null
	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
		if (cl.worldmodel->textures[i])
			cl.worldmodel->textures[i]->texturechain = NULL;

	// rebuild chains

#if 1
	//iterate through surfaces one node at a time to rebuild chains
	//need to do it this way if we want to work with tyrann's skip removal tool
	//becuase his tool doesn't actually remove the surfaces from the bsp surfaces lump
	//nor does it remove references to them in each leaf's marksurfaces list
	for (i=0, node = cl.worldmodel->nodes ; i<cl.worldmodel->numnodes ; i++, node++)
		for (j=0, surf=&cl.worldmodel->surfaces[node->firstsurface] ; j<node->numsurfaces ; j++, surf++)
			if (surf->visframe == r_visframecount)
			{
				surf->texturechain = surf->texinfo->texture->texturechain;
				surf->texinfo->texture->texturechain = surf;
			}
#else
	//the old way
	surf = &cl.worldmodel->surfaces[cl.worldmodel->firstmodelsurface];
	for (i=0 ; i<cl.worldmodel->nummodelsurfaces ; i++, surf++)
	{
		if (surf->visframe == r_visframecount)
		{
			surf->texturechain = surf->texinfo->texture->texturechain;
			surf->texinfo->texture->texturechain = surf;
		}
	}
#endif
}

bool R_BackFaceCull (msurface_t *surf)
{
	double dot;

	switch (surf->plane->type)
	{
	case PLANE_X:
		dot = r_refdef.vieworg[0]-surf->plane->dist;
		break;
	case PLANE_Y:
		dot = r_refdef.vieworg[1]-surf->plane->dist;
		break;
	case PLANE_Z:
		dot = r_refdef.vieworg[2]-surf->plane->dist;
		break;
	default:
		dot = Math_DotProduct(r_refdef.vieworg,surf->plane->normal)-surf->plane->dist;
		break;
	}

	if((dot < 0) ^ !!(surf->flags & SURF_PLANEBACK))
		return true;

	return false;
}

void R_CullSurfaces (void)
{
	msurface_t		*s;
	unsigned int	i;

	if (!r_drawworld_cheatsafe)
		return;

	s = &cl.worldmodel->surfaces[cl.worldmodel->firstmodelsurface];
	for (i=0 ; i<cl.worldmodel->nummodelsurfaces ; i++, s++)
	{
		if (s->visframe == r_visframecount)
		{
			if (R_CullBox(s->mins, s->maxs) || R_BackFaceCull (s))
				s->culled = true;
			else
			{
				s->culled = false;
				rs_brushpolys++; //count wpolys here
#if 0
				if (s->texinfo->texture->warpimage)
					s->texinfo->texture->update_warp = true;
#endif
			}
		}
	}
}

void R_BuildLightmapChains (void)
{
	msurface_t *s;
	int i;

	// clear lightmap chains (already done in r_marksurfaces, but clearing them here to be safe becuase of r_stereo)
	memset (lightmap_polys, 0, sizeof(lightmap_polys));

	// now rebuild them
	s = &cl.worldmodel->surfaces[cl.worldmodel->firstmodelsurface];
	for (i=0 ; i<cl.worldmodel->nummodelsurfaces ; i++, s++)
		if (s->visframe == r_visframecount && !R_CullBox(s->mins, s->maxs) && !R_BackFaceCull (s))
			R_RenderDynamicLightmaps (s);
}

//==============================================================================
//
// DRAW CHAINS
//
//==============================================================================

void R_DrawTextureChains_ShowTris (void)
{
	int			i;
	msurface_t	*s;
	texture_t	*t;
	glpoly_t	*p;

	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		t = cl.worldmodel->textures[i];
		if (!t)
			continue;

		if (r_oldwater.value && t->texturechain && (t->texturechain->flags & SURF_DRAWTURB))
		{
			for (s = t->texturechain; s; s = s->texturechain)
				if (!s->culled)
					for (p = s->polys->next; p; p = p->next)
						DrawGLPoly (p);
		}
		else
		{
			for (s = t->texturechain; s; s = s->texturechain)
				if (!s->culled)
					DrawGLPoly (s->polys);
		}
	}
}

void R_DrawTextureChains_Drawflat (void)
{
	int			i;
	msurface_t	*s;
	texture_t	*t;
	glpoly_t	*p;

	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		t = cl.worldmodel->textures[i];
		if (!t)
			continue;

		if (r_oldwater.value && t->texturechain && (t->texturechain->flags & SURF_DRAWTURB))
		{
			for (s = t->texturechain; s; s = s->texturechain)
				if (!s->culled)
					for (p = s->polys->next; p; p = p->next)
					{
						DrawGLPoly (p);
						rs_brushpasses++;
					}
		}
		else
		{
			for (s = t->texturechain; s; s = s->texturechain)
				if (!s->culled)
				{
					DrawGLPoly (s->polys);
					rs_brushpasses++;
				}
		}
	}
	glColor3f (1,1,1);
	srand ((int) (cl.time * 1000));
}

void R_DrawTextureChains_Glow (void)
{
#if 0
	int			i;
	msurface_t	*s;
	texture_t	*t;
	gltexture_t	*glt;
	bool		bBound;

	for(i = 0; i < cl.worldmodel->numtextures; i++)
	{
		t = cl.worldmodel->textures[i];

		if (!t || !t->texturechain)
			continue;

		glt = R_TextureAnimation(t,0)->fullbright;
		if(!glt)
			continue;

		bBound = false;

		for(s = t->texturechain; s; s = s->texturechain)
			if(!s->culled)
			{
				if(!bBound) // only bind once we are sure we need this texture
				{
					Video_SetTexture(glt);
					bBound = true;
				}

				DrawGLPoly(s->polys);

				rs_brushpasses++;
			}
	}
#endif
}

/*	Draws surfs whose textures were missing from the BSP
*/
void R_DrawTextureChains_NoTexture (void)
{
#if 0
	int			i;
	bool		bTextureBound;
	msurface_t	*s;
	texture_t	*t;

	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		t = cl.worldmodel->textures[i];

		if (!t || !t->texturechain || !(t->texturechain->flags & SURF_NOTEXTURE))
			continue;

		bTextureBound = false;

		for (s = t->texturechain; s; s = s->texturechain)
			if (!s->culled)
			{
				if(!bTextureBound) //only bind once we are sure we need this texture
				{
					Video_SetTexture(Material_Get(t->iAssignedTexture)->msSkin[0].gDiffuseTexture);

					bTextureBound = true;
				}

				DrawGLPoly(s->polys);

				rs_brushpasses++;
			}
	}
#endif
}

void R_DrawTextureChains_TextureOnly (void)
{
	int			i;
	bool		bTextureBound;
	msurface_t	*s;
	texture_t	*t;

	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		t = cl.worldmodel->textures[i];

		if (!t || !t->texturechain || t->texturechain->flags & (SURF_DRAWTURB | SURF_DRAWSKY))
			continue;

		bTextureBound = false;

		for (s = t->texturechain; s; s = s->texturechain)
			if (!s->culled)
			{
				if(!bTextureBound) //only bind once we are sure we need this texture
				{
					Video_SetTexture(Material_Get((R_TextureAnimation(t,0))->iAssignedMaterial)->msSkin[0].gDiffuseTexture);

					bTextureBound = true;
				}

				R_RenderDynamicLightmaps(s); //adds to lightmap chain

				DrawGLPoly(s->polys);

				rs_brushpasses++;
			}
	}
}

void World_DrawWaterTextureChains(void)
{
	int			i;
	bool		bTextureBound;
	msurface_t	*s;
	texture_t	*t;
	glpoly_t	*p;

	if (r_drawflat_cheatsafe || r_lightmap_cheatsafe || !r_drawworld_cheatsafe)
		return;

	Video_ResetCapabilities(false);

	if (r_wateralpha.value < 1.0)
	{
		Video_EnableCapabilities(VIDEO_BLEND);
		Video_SetBlend(VIDEO_BLEND_IGNORE,VIDEO_DEPTH_FALSE);
	}

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_RGB_SCALE,4);

	if (r_oldwater.value)
	{
		for (i=0 ; i<cl.worldmodel->numtextures ; i++)
		{
			t = cl.worldmodel->textures[i];
			if (!t || !t->texturechain || !(t->texturechain->flags & SURF_DRAWTURB))
				continue;

			bTextureBound = false;

			for (s = t->texturechain; s; s = s->texturechain)
				if(!s->culled)
				{
					if(!bTextureBound)
					{
						// Only bind once we are sure we need this texture
						Video_SetTexture(Material_Get(t->iAssignedMaterial)->msSkin[0].gDiffuseTexture);

						bTextureBound = true;
					}

					for(p = s->polys->next; p; p = p->next)
					{
						Warp_DrawWaterPoly(p);
						rs_brushpasses++;
					}
				}
		}
	}
#if 0
	else
	{
		for (i=0 ; i<cl.worldmodel->numtextures ; i++)
		{
			t = cl.worldmodel->textures[i];
			if (!t || !t->texturechain || !(t->texturechain->flags & SURF_DRAWTURB))
				continue;

			bTextureBound = false;

			for (s = t->texturechain; s; s = s->texturechain)
				if (!s->culled)
				{
					if(!bTextureBound) //only bind once we are sure we need this texture
					{
						Video_SetTexture(t->warpimage);

						bTextureBound = true;
					}

					DrawGLPoly(s->polys);

					rs_brushpasses++;
				}
		}
	}
#endif

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	Video_ResetCapabilities(true);
}

void R_DrawTextureChains_White (void)
{
	int			i;
	msurface_t	*s;
	texture_t	*t;

	glDisable (GL_TEXTURE_2D);
	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		t = cl.worldmodel->textures[i];

		if (!t || !t->texturechain || !(t->texturechain->flags & SURF_DRAWTILED))
			continue;

		for (s = t->texturechain; s; s = s->texturechain)
			if (!s->culled)
			{
				Video_SetTexture(Material_Get((R_TextureAnimation(t,0))->iAssignedMaterial)->msSkin[0].gDiffuseTexture);
				DrawGLPoly (s->polys);
				rs_brushpasses++;
			}
	}
	glEnable (GL_TEXTURE_2D);
}

void R_DrawLightmapChains (void)
{
	int			i,j;
	float		*v;
	glpoly_t	*p;

	for(i = 0; i < MAX_LIGHTMAPS; i++)
	{
		if(!lightmap_polys[i])
			continue;

		Video_SetTexture(lightmap_textures[i]);

		R_UploadLightmap(i);

		for (p = lightmap_polys[i]; p; p=p->chain)
		{
			glBegin(GL_TRIANGLE_FAN);

			v = p->verts[0];
			for(j = 0; j < p->numverts; j++,v += VERTEXSIZE)
			{
				glTexCoord2fv(v+5);
				glVertex3fv(v);
			}

			glEnd();

			rs_brushpasses++;
		}
	}
}

void World_Draw(void)
{
	VideoObject_t	*voWorld;

	if(!r_drawworld_cheatsafe)
		return;

    Video_ResetCapabilities(false);

	if(r_drawflat_cheatsafe)
	{
		Video_DisableCapabilities(VIDEO_TEXTURE_2D);

		R_DrawTextureChains_Drawflat();

		Video_ResetCapabilities(true);
		return;
	}
	else if(r_fullbright_cheatsafe)
	{
		R_DrawTextureChains_TextureOnly ();

        Video_EnableCapabilities(VIDEO_BLEND);
		Video_SetBlend(VIDEO_BLEND_ONE,VIDEO_DEPTH_FALSE);

		Fog_StartAdditive();

		R_DrawTextureChains_Glow();

		Fog_StopAdditive();

		Video_ResetCapabilities(true);
		return;
	}
	else if(r_lightmap_cheatsafe)
	{
		R_BuildLightmapChains();
		R_DrawLightmapChains();
		R_DrawTextureChains_White();

		Video_ResetCapabilities(true);
		return;
	}

	//R_DrawTextureChains_NoTexture();

	{
		int			i, j;
		msurface_t	*s;
		texture_t	*t;
		float		*v;
		bool		bBound;

		for(i = 0; i < cl.worldmodel->numtextures; i++)
		{
			t = cl.worldmodel->textures[i];
			if(!t || !t->texturechain || t->texturechain->flags & (SURF_DRAWTILED | SURF_NOTEXTURE))
				continue;

			bBound = false;
			for(s = t->texturechain; s; s = s->texturechain)
				if(!s->culled)
				{
					Material_t *mMaterial = Material_Get(R_TextureAnimation(t, 0)->iAssignedMaterial);

#if 0
					if(!bBound) //only bind once we are sure we need this texture
					{
						Video_SetTexture(mMaterial->msSkin[0].gDiffuseTexture);
						Video_SelectTexture(1);
						Video_EnableCapabilities(VIDEO_TEXTURE_2D);

						bBound = true;
					}
#endif

#if 0
					R_RenderDynamicLightmaps(s);

					Video_SetTexture(lightmap_textures[s->lightmaptexturenum]);

					R_UploadLightmap(s->lightmaptexturenum);
#endif

					voWorld = (VideoObject_t*)Hunk_TempAlloc(s->polys->numverts*sizeof(VideoObject_t));
					if (!voWorld)
						Sys_Error("Failed to allocate world object!\n");
						
					v = s->polys->verts[0];
					for (j = 0; j < s->polys->numverts; j++, v += VERTEXSIZE)
					{
						Math_Vector2Copy((v + 3), voWorld[j].vTextureCoord[0]);
						Math_Vector2Copy((v + 5), voWorld[j].vTextureCoord[1]);
						Math_VectorCopy(v, voWorld[j].vVertex);
						Math_Vector4Set(1.0f, voWorld[j].vColour);
					}

					Video_DrawObject(voWorld, VIDEO_PRIMITIVE_TRIANGLE_FAN, s->polys->numverts, mMaterial, 0);

					rs_brushpasses++;
				}
		}
	}

	Video_ResetCapabilities(true);
}
