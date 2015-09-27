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

/*
	World
*/

#include "EngineVideo.h"
#include "EngineVideoEFrag.h"

extern cvar_t gl_fullbrights, r_drawflat, r_oldwater, r_oldskyleaf, r_showtris; //johnfitz

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
			}
		}
	}
}

void R_BuildLightmapChains (void)
{
	msurface_t		*s;
	unsigned int	i;

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

void R_DrawTextureChains_Drawflat (void)
{
	glColor3f (1,1,1);
	srand ((int) (cl.time * 1000));
}

void Surface_DrawMirror(msurface_t *Surface)
{
	float fDirection;

	// Prevent recursion...
	if (r_refdef.bMirror)
		return;

	// Copy the matrix.
	memcpy(r_base_world_matrix, r_world_matrix, sizeof(r_base_world_matrix));

#if 1
	fDirection = Math_DotProduct(r_refdef.vieworg, Surface->plane->normal) - Surface->plane->dist;
	Math_VectorMA(r_refdef.vieworg, -2 * fDirection, Surface->plane->normal, r_refdef.vieworg);

//	fDirection = Math_DotProduct(vpn, Surface->plane->normal);
//	Math_VectorMA(vpn, -2 * fDirection, Surface->plane->normal, vpn);

//	r_refdef.viewangles[0] = -asinf(vpn[2]) / pMath_PI * 180.0f;
///	r_refdef.viewangles[1] = atan2f(vpn[1], vpn[0]) / pMath_PI * 180.0f;
	//r_refdef.viewangles[2] = -r_refdef.viewangles[2];
#else
//	Math_VectorMA(r_refdef.vieworg, -2, Surface->plane->normal, r_refdef.vieworg);
//	Math_VectorMA(vpn, -2, Surface->plane->normal, vpn);

	//Math_VectorInverse(r_refdef.viewangles);
//	r_refdef.viewangles[0] = -asinf(vpn[2]) / pMath_PI * 180.0f;
//	r_refdef.viewangles[1] = atan2f(vpn[1], vpn[0]) / pMath_PI * 180.0f;
//	r_refdef.viewangles[2] = -r_refdef.viewangles[2];
#endif

	VideoLayer_Enable(VIDEO_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 255);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilMask(255);
	Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);

	Video_DrawSurface(Surface, 0, mColour, 0);

	glStencilFunc(GL_EQUAL, 1, 255);
	glStencilMask(0);
	Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_TRUE);

	ClientEntity_t *ViewEntity = &cl_entities[cl.viewentity];
	if (cl_numvisedicts < MAX_VISEDICTS)
	{
		cl_visedicts[cl_numvisedicts] = ViewEntity;
		cl_numvisedicts++;
	}

	r_refdef.bMirror = true;
	{
		R_SetupScene();

		glDepthRange(0, 0.5);
		glScalef(1.0f, -1.0f, 1.0f);
		
		World_Draw();

		glDepthRange(0, 1);
	}
	r_refdef.bMirror = false;

	if (cl_numvisedicts < (MAX_VISEDICTS-1))
	{
		cl_visedicts[cl_numvisedicts] = NULL;
		cl_numvisedicts--;
	}

	VideoLayer_Disable(VIDEO_STENCIL_TEST);

	// Restore the original matrix.
	glLoadMatrixf(r_base_world_matrix);

#if 0
	// Blend the final surface on top.
	VideoLayer_Enable(VIDEO_BLEND);
	Video_DrawSurface(Surface, 0.5f, mColour, 1);
	VideoLayer_Disable(VIDEO_BLEND);
#endif
}

void World_DrawMirrors(void)
{
}

void World_DrawWaterTextureChains(void)
{
	int			i;
	msurface_t	*s;
	texture_t	*t;
	glpoly_t	*p;

	if (r_drawflat_cheatsafe || r_lightmap_cheatsafe || !r_drawworld_cheatsafe)
		return;

	Video_ResetCapabilities(false);

	Video_EnableCapabilities(VIDEO_TEXTURE_2D|VIDEO_BLEND);

	for (i = 0; i < cl.worldmodel->numtextures; i++)
	{
		t = cl.worldmodel->textures[i];
		if (!t || !t->texturechain || !(t->texturechain->flags & SURF_DRAWTURB))
			continue;

		if (t->mAssignedMaterial->fAlpha < 1.0)
			Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_TRUE);

		for (s = t->texturechain; s; s = s->texturechain)
			if(!s->culled)
			{
				Video_DrawMaterial(t->mAssignedMaterial, 0, 0, 0, 0, false);

				for(p = s->polys->next; p; p = p->next)
				{
					Warp_DrawWaterPoly(p, t->mAssignedMaterial);

					rs_brushpasses++;
				}

				Video_DrawMaterial(t->mAssignedMaterial, 0, 0, 0, 0, true);
			}
	}

	Video_ResetCapabilities(true);
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
	int			i;
	msurface_t	*s;
	texture_t	*t;

	if(!r_drawworld_cheatsafe)
		return;

	Video_ResetCapabilities(false);

	for(i = 0; i < cl.worldmodel->numtextures; i++)
	{
		t = cl.worldmodel->textures[i];
		if(!t || !t->texturechain || t->texturechain->flags & (SURF_DRAWTILED | SURF_NOTEXTURE))
			continue;

		t->mAssignedMaterial->bBind = true;

		for(s = t->texturechain; s; s = s->texturechain)
			if(!s->culled)
			{
				if (t->mAssignedMaterial->bBind && !r_showtris.bValue)
				{
					Video_SelectTexture(VIDEO_TEXTURE_LIGHT);
					Video_EnableCapabilities(VIDEO_TEXTURE_2D);

					t->mAssignedMaterial->bBind = false;
				}

				if (!r_showtris.bValue)
				{
					Video_SelectTexture(VIDEO_TEXTURE_LIGHT);

					Video_SetTexture(lightmap_textures[s->lightmaptexturenum]);
					R_RenderDynamicLightmaps(s);
					R_UploadLightmap(s->lightmaptexturenum);

					Video_SelectTexture(VIDEO_TEXTURE_DIFFUSE);
				}

				Video_DrawSurface(s, 1.0f, t->mAssignedMaterial, 0);

				rs_brushpasses++;
			}
	}

	// Disable light texture
	Video_SelectTexture(VIDEO_TEXTURE_LIGHT);
	Video_DisableCapabilities(VIDEO_TEXTURE_2D);

	Video_ResetCapabilities(true);
}
