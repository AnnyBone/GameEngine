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

#include "EngineScript.h"
#include "video.h"

#include "platform_model.h"

/*
	Model Loading and Caching
	Models are the only shared resource between a client and server running on the same machine.
*/

model_t	*loadmodel;
char	loadname[32];	// for hunk tags

void Model_LoadBSP(model_t *mod, void *buffer);
void Model_LoadMD2(model_t *mod,void *buffer);

model_t *Model_Load(model_t *mod);

uint8_t mod_novis[BSP_MAX_LEAFS/8];

#define	MAX_MOD_KNOWN	2048 //johnfitz -- was 512
model_t	mod_known[MAX_MOD_KNOWN];
int		mod_numknown;

texture_t	*r_notexture_mip; //johnfitz -- moved here from r_main.c
texture_t	*r_notexture_mip2; //johnfitz -- used for non-lightmapped surfs with a missing texture

void Model_Initialize(void)
{
	memset(mod_novis,0xff,sizeof(mod_novis));

	//johnfitz -- create notexture miptex
	r_notexture_mip = (texture_t*)Hunk_AllocName(sizeof(texture_t),"r_notexture_mip");
	strcpy(r_notexture_mip->name, "notexture");
	r_notexture_mip->height = r_notexture_mip->width = 32;

	r_notexture_mip2 = (texture_t*)Hunk_AllocName (sizeof(texture_t), "r_notexture_mip2");
	strcpy(r_notexture_mip2->name, "notexture2");
	r_notexture_mip2->height = r_notexture_mip2->width = 32;
	//johnfitz
}

/*	Caches the data if needed
*/
void *Mod_Extradata (model_t *mod)
{
	void *r;

	r = Cache_Check(&mod->cache);
	if(r)
		return r;

	Model_Load(mod);
	if(!mod->cache.data)
		Sys_Error ("Mod_Extradata: caching failed");

	return mod->cache.data;
}

mleaf_t *Mod_PointInLeaf (MathVector3f_t p, model_t *model)
{
	mnode_t		*node;
	float		d;
	mplane_t	*plane;

	if (!model || !model->nodes)
	{
		Sys_Error ("Mod_PointInLeaf: bad model");
		return NULL;
	}

	node = model->nodes;
	for(;;)
	{
		if(node->contents < 0)
			// [5/3/2013] Break here so we DO reach the end and don't create dumb warnings. Seriously. ~hogsy
			break;

		plane = node->plane;
		d = Math_DotProduct (p,plane->normal) - plane->dist;
		if (d > 0)
			node = node->children[0];
		else
			node = node->children[1];
	}

	return (mleaf_t*)node;
}

byte *Mod_DecompressVis (byte *in, model_t *model)
{
	static byte	decompressed[BSP_MAX_LEAFS/8];
	int		c;
	byte	*out;
	int		row;

	row = (model->numleafs+7)>>3;
	out = decompressed;

	if (!in)
	{	// no vis info, so make all visible
		while (row)
		{
			*out++ = 0xff;
			row--;
		}
		return decompressed;
	}

	do
	{
		if (*in)
		{
			*out++ = *in++;
			continue;
		}

		c = in[1];
		in += 2;
		while (c)
		{
			*out++ = 0;
			c--;
		}
	} while (out - decompressed < row);

	return decompressed;
}

byte *Mod_LeafPVS (mleaf_t *leaf, model_t *model)
{
	if(leaf == model->leafs)
		return mod_novis;

	return Mod_DecompressVis (leaf->compressed_vis, model);
}

void Model_ClearAll(void)
{
	int		i;
	model_t	*mModel;

	for(i = 0,mModel = mod_known; i < mod_numknown; i++,mModel++)
		if (mModel->type != MODEL_TYPE_MD2)
		{
			mModel->bNeedLoad = true;

			TexMgr_FreeTexturesForOwner(mModel); //johnfitz
		}
}

model_t *Model_FindName(const char *cName)
{
	int		i;
	model_t	*mModel;

	if(!cName[0])
		Sys_Error ("Model_FindName: NULL name");

	// Search the currently loaded models
	for(i = 0,mModel = mod_known; i < mod_numknown; i++,mModel++)
		if(!strcmp(mModel->name,cName))
			break;

	if(i == mod_numknown)
	{
		if (mod_numknown == MAX_MOD_KNOWN)
			Sys_Error ("mod_numknown == MAX_MOD_KNOWN");

		strcpy(mModel->name, cName);

		mModel->bNeedLoad = true;

		mod_numknown++;
	}

	return mModel;
}

void Model_Touch(char *cName)
{
	model_t	*mModel;

	mModel = Model_FindName(cName);
	if(!mModel->bNeedLoad)
		if (mModel->type == MODEL_TYPE_MD2)
			Cache_Check(&mModel->cache);
}

typedef struct
{
	const char *extension;

	int type;

	bool (*Function)(model_t *model);
} ModelLoadInterface;

bool Model_LoadOBJ(model_t *model);

ModelLoadInterface model_formatlist[] =
{
	{ U3D_FILE_EXTENSION, MODEL_TYPE_STATIC, ModelU3D_Load },
	{ "obj", MODEL_TYPE_STATIC, Model_LoadOBJ }
};

/*	Loads a model into the cache
*/
model_t *Model_Load(model_t *model)
{
	char	exten[64];
	void	*d;
	int		i;

	if (!model->bNeedLoad)
	{
		if (model->type == MODEL_TYPE_MD2)
		{
			d = Cache_Check(&model->cache);
			if (d)
				return model;
		}
		else
			return model;
	}

	// Load the file
	void *buf = COM_LoadHeapFile(model->name);
	if(!buf)
	{
		Con_Warning("Model was not found! (%s)\n", model->name);
		return NULL;
	}

	// Allocate a new model
	COM_FileBase(model->name, loadname);

	loadmodel = model;

	// Call the apropriate loader
	model->bNeedLoad = false;

	switch (LittleLong(*(unsigned*)buf))
	{
	case MD2_HEADER:
		Model_LoadMD2(model, buf);
		break;
	case BSP_HEADER:
		Model_LoadBSP(model, buf);
		break;
	default:
		// Grab the file extension, so we can check for formats that don't have idents.
		strncpy(exten, COM_FileExtension(model->name), sizeof(exten));
		if (exten[0] != ' ')
		{
			for (i = 0; i < plArrayElements(model_formatlist); i++)
			{
				if (!strcmp(model_formatlist[i].extension, exten))
				{
					// Set the default type, we can change later.
					model->type = (ModelType_t)model_formatlist[i].type;

					if (model_formatlist[i].Function(model))
						return model;

					break;
				}
			}
		}

		// If we reach this point, definately not supported.
		Con_Warning("Failed to load model type! (%s)\n", model->name);

		model = NULL;
	}

	free(buf);

	return model;
}

/*	Loads in a model for the given name
*/
model_t *Mod_ForName(const char *cName)
{
	return Model_Load(Model_FindName(cName));
}

/*
===============================================================================

					BRUSHMODEL LOADING

===============================================================================
*/

uint8_t	*mod_base;

void Model_LoadBSPTextures(BSPLump_t *blLump)
{
	texture_t		*tTexture;
	Material_t		*mAssignedMaterial;
	miptex_t		*miptexture;
	dmiptexlump_t	*mLump = NULL;
	int				i, iTextures = 0;

	if(!blLump->iFileLength)
		Con_Warning("No textures in BSP file!\n");
	else
	{
		mLump = (dmiptexlump_t*)(mod_base + blLump->iFileOffset);
		mLump->nummiptex = LittleLong(mLump->nummiptex);

		iTextures = mLump->nummiptex;
	}

	loadmodel->numtextures	= iTextures+2;
	loadmodel->textures		= (texture_t**)Hunk_AllocName(loadmodel->numtextures*sizeof(*loadmodel->textures), loadname);

	for(i = 0; i < iTextures; i++)
	{
		mLump->dataofs[i] = LittleLong(mLump->dataofs[i]);
		if(mLump->dataofs[i] == -1)
			continue;

		miptexture = (miptex_t*)((uint8_t*)mLump + mLump->dataofs[i]);

		// Scales are checked automatically by material system, so that original check has been removed.

		tTexture = (texture_t*)Hunk_AllocName(sizeof(texture_t), loadname);
		loadmodel->textures[i] = tTexture;

		memcpy(tTexture->name, miptexture->name, sizeof(tTexture->name));
		tTexture->width = LittleLong(miptexture->width);
		tTexture->height = LittleLong(miptexture->height);

		// Remove special characters.
		if (tTexture->name[0] == '*')
			strcpy(tTexture->name, tTexture->name + 1);

		FileSystem_UpdatePath(tTexture->name);

		// Don't bother loading textures for dedicated servers.
		if (!bIsDedicated)
		{
			mAssignedMaterial = Material_Load(tTexture->name);
			if (mAssignedMaterial)
				tTexture->material = mAssignedMaterial;
			else
				tTexture->material = g_mMissingMaterial;
		}
	}

	loadmodel->textures[loadmodel->numtextures - 2] = r_notexture_mip;	//for lightmapped surfs
	loadmodel->textures[loadmodel->numtextures - 1] = r_notexture_mip2; //for SURF_DRAWTILED surfs
}

void Model_LoadBSPLighting(BSPLump_t *blLump)
{
	if(!blLump->iFileLength)
		return;

	loadmodel->lightdata = (uint8_t*)Hunk_Alloc(blLump->iFileLength);

	memcpy(loadmodel->lightdata,mod_base+blLump->iFileOffset,blLump->iFileLength);
}

void Model_LoadBSPVisibility(BSPLump_t *blLump)
{
	if(!blLump->iFileLength)
	{
		loadmodel->visdata = NULL;
		return;
	}

	loadmodel->visdata = (uint8_t*)Hunk_AllocName(blLump->iFileLength, loadname);
	memcpy(loadmodel->visdata, mod_base + blLump->iFileOffset, blLump->iFileLength);
}

void Model_LoadBSPEntities(BSPLump_t *blLump)
{
	if(!blLump->iFileLength)
	{
		loadmodel->entities = NULL;
		return;
	}

	loadmodel->entities = (char*)Hunk_AllocName(blLump->iFileLength,loadname);

	memcpy(loadmodel->entities,mod_base+blLump->iFileOffset,blLump->iFileLength);
}

void Model_LoadBSPVertexes(BSPLump_t *blLump)
{
	BSPVertex_t	*in;
	BSPVertex_t	*out;
	int			i, count;

	in = (BSPVertex_t *)(mod_base+blLump->iFileOffset);
	if(blLump->iFileLength%sizeof(*in))
		Sys_Error ("Model_LoadBSPVertexes: funny lump size in %s",loadmodel->name);

	count	= blLump->iFileLength/sizeof(*in);
	out		= (BSPVertex_t*)Hunk_AllocName(count*sizeof(*out),loadname);

	loadmodel->vertexes		= out;
	loadmodel->numvertexes	= count;

	for(i = 0; i < count; i++,in++,out++)
	{
		out->fPoint[0]	= LittleFloat(in->fPoint[0]);
		out->fPoint[1]	= LittleFloat(in->fPoint[1]);
		out->fPoint[2]	= LittleFloat(in->fPoint[2]);
	}
}

void Model_LoadBSPEdges(BSPLump_t *blLump)
{
	BSPEdge_t	*beEdge;
	medge_t		*out;
	int 		i,count;

	beEdge = (BSPEdge_t*)(mod_base+blLump->iFileOffset);
	if(blLump->iFileLength%sizeof(*beEdge))
		Sys_Error ("Model_LoadBSPEdges: funny lump size in %s",loadmodel->name);

	count	= blLump->iFileLength/sizeof(*beEdge);
	out		= (medge_t*)Hunk_AllocName((count+1)*sizeof(*out),loadname);

	loadmodel->edges	= out;
	loadmodel->numedges = count;

	for(i = 0; i < count; i++,beEdge++,out++)
	{
		out->v[0] = (unsigned short)LittleShort(beEdge->v[0]);
		out->v[1] = (unsigned short)LittleShort(beEdge->v[1]);
	}
}

void Model_LoadBSPTextureInfo(BSPLump_t *blLump)
{
	BSPTextureInfo_t	*in;
	mtexinfo_t			*out;
	int 				i, j, count;
	unsigned int		miptex;
	float				len1, len2;
	int					missing = 0; //johnfitz

	in = (BSPTextureInfo_t *)(mod_base + blLump->iFileOffset);
	if (blLump->iFileLength % sizeof(*in))
		Sys_Error ("Model_LoadBSPTextureInfo: funny lump size in %s",loadmodel->name);
	count = blLump->iFileLength / sizeof(*in);
	out = (mtexinfo_t*)Hunk_AllocName ( count*sizeof(*out), loadname);

	loadmodel->texinfo		= out;
	loadmodel->numtexinfo	= count;

	for ( i=0 ; i<count ; i++, in++, out++)
	{
		for (j=0 ; j<8 ; j++)
			out->vecs[0][j] = LittleFloat (in->v[0][j]);

		len1 = plLengthf(out->vecs[0]);
		len2 = plLengthf(out->vecs[1]);
		len1 = (len1 + len2)/2;

		miptex = LittleLong (in->iMipTex);
		out->flags = LittleLong (in->iFlags);

		//johnfitz -- rewrote this section
		if (miptex >= loadmodel->numtextures-1 || !loadmodel->textures[miptex])
		{
			if (out->flags & BSP_TEXTURE_SKY)
				out->texture = loadmodel->textures[loadmodel->numtextures-1];
			else
				out->texture = loadmodel->textures[loadmodel->numtextures-2];
			out->flags |= BSP_TEXTURE_MISSING;
			missing++;
		}
		else
			out->texture = loadmodel->textures[miptex];
		//johnfitz
	}

	//johnfitz: report missing textures
	if (missing && loadmodel->numtextures > 1)
		Con_Warning("Textures missing from BSP file! (%i)\n",missing);
	//johnfitz
}

/*	Fills in s->texturemins[] and s->extents[]
*/
void CalcSurfaceExtents (msurface_t *s)
{
	float	mins[2], maxs[2];
	double	val;
	int		i,j, e;
	BSPVertex_t	*v;
	mtexinfo_t	*tex;
	int		bmins[2], bmaxs[2];

	mins[0] = mins[1] = 999999;
	maxs[0] = maxs[1] = -99999;

	tex = s->texinfo;

	for (i=0 ; i<s->numedges ; i++)
	{
		e = loadmodel->surfedges[s->firstedge+i];
		if (e >= 0)
			v = &loadmodel->vertexes[loadmodel->edges[e].v[0]];
		else
			v = &loadmodel->vertexes[loadmodel->edges[-e].v[1]];

		for (j=0 ; j<2 ; j++)
		{
			// Double cast, suggestion from LordHavoc.
			val =	(double)	v->fPoint[0]*tex->vecs[j][0]+
								v->fPoint[1]*tex->vecs[j][1]+
								v->fPoint[2]*tex->vecs[j][2]+
								tex->vecs[j][3];
			if (val < mins[j])
				mins[j] = val;
			if (val > maxs[j])
				maxs[j] = val;
		}
	}

	for (i=0 ; i<2 ; i++)
	{
		bmins[i] = floor(mins[i]/16);
		bmaxs[i] = ceil(maxs[i]/16);

		s->texturemins[i] = bmins[i]*16;
		s->extents[i] = (bmaxs[i]-bmins[i])*16;

		if (!(tex->flags & BSP_TEXTURE_SKY) && s->extents[i] > 2000) //johnfitz -- was 512 in glquake, 256 in winquake
			Con_Warning("Bad surface extents\n");
	}
}

/*
================
Mod_PolyForUnlitSurface -- johnfitz -- creates polys for unlightmapped surfaces (sky and water)

TODO: merge this into BuildSurfaceDisplayList?
================
*/
void Mod_PolyForUnlitSurface (msurface_t *fa)
{
	plVector3f_t	verts[64];
	int				numverts, i, lindex;
	float			*vec;
	glpoly_t		*poly;
	float			texscale;

	if (fa->flags & (SURF_DRAWTURB | SURF_DRAWSKY))
		texscale = (1.0/128.0); //warp animation repeats every 128
	else
		texscale = (1.0/32.0); //to match r_notexture_mip

	// convert edges back to a normal polygon
	numverts = 0;
	for (i=0 ; i<fa->numedges ; i++)
	{
		lindex = loadmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
			vec = loadmodel->vertexes[loadmodel->edges[lindex].v[0]].fPoint;
		else
			vec = loadmodel->vertexes[loadmodel->edges[-lindex].v[1]].fPoint;
		Math_VectorCopy(vec,verts[numverts]);
		numverts++;
	}

	//create the poly
	poly = (glpoly_t*)Hunk_Alloc (sizeof(glpoly_t) + (numverts-4) * VERTEXSIZE*sizeof(float));
	poly->next = NULL;
	fa->polys = poly;
	poly->numverts = numverts;

	for (i=0, vec=(float *)verts; i<numverts; i++, vec+= 3)
	{
		Math_VectorCopy (vec, poly->verts[i]);
		poly->verts[i][3] = Math_DotProduct(vec, fa->texinfo->vecs[0]) * texscale;
		poly->verts[i][4] = Math_DotProduct(vec, fa->texinfo->vecs[1]) * texscale;
	}
}

void Mod_CalcSurfaceBounds(msurface_t *s)
{
	int			i, e;
	BSPVertex_t	*v;

	s->mins[0] = s->mins[1] = s->mins[2] = 9999;
	s->maxs[0] = s->maxs[1] = s->maxs[2] = -9999;

	for (i=0 ; i<s->numedges ; i++)
	{
		e = loadmodel->surfedges[s->firstedge+i];
		if (e >= 0)
			v = &loadmodel->vertexes[loadmodel->edges[e].v[0]];
		else
			v = &loadmodel->vertexes[loadmodel->edges[-e].v[1]];

		if (s->mins[0] > v->fPoint[0])
			s->mins[0] = v->fPoint[0];
		if (s->mins[1] > v->fPoint[1])
			s->mins[1] = v->fPoint[1];
		if (s->mins[2] > v->fPoint[2])
			s->mins[2] = v->fPoint[2];

		if (s->maxs[0] < v->fPoint[0])
			s->maxs[0] = v->fPoint[0];
		if (s->maxs[1] < v->fPoint[1])
			s->maxs[1] = v->fPoint[1];
		if (s->maxs[2] < v->fPoint[2])
			s->maxs[2] = v->fPoint[2];
	}
}

void GL_SubdivideSurface(msurface_t *fa);

void Model_LoadBSPFaces(BSPLump_t *blLump)
{
	Material_t	*mMaterial;
	BSPFace_t	*in;
	msurface_t 	*out;
	int			i, count, surfnum;
	int			planenum, side;

	in = (BSPFace_t *)(mod_base + blLump->iFileOffset);
	if (blLump->iFileLength % sizeof(*in))
	{
		Sys_Error ("Model_LoadBSPFaces: funny lump size in %s",loadmodel->name);
		return;
	}

	count = blLump->iFileLength / sizeof(*in);

	out = (msurface_t*)Hunk_AllocName ( count*sizeof(*out), loadname);
	if(!out)
	{
		Sys_Error("Failed to allocate surface!\n");
		return;
	}

	loadmodel->surfaces = out;
	loadmodel->numsurfaces = count;

	for ( surfnum=0 ; surfnum<count ; surfnum++, in++, out++)
	{
		out->firstedge	= LittleLong(in->iFirstEdge);
		out->numedges	= LittleLong(in->iNumEdges);
		out->flags		= 0;

		planenum = LittleLong(in->iPlaneNum);

		side = LittleLong(in->iSide);
		if (side)
			out->flags |= SURF_PLANEBACK;

		out->plane = loadmodel->planes + planenum;

		out->texinfo = loadmodel->texinfo+LittleLong(in->iTexInfo);

		CalcSurfaceExtents (out);

		Mod_CalcSurfaceBounds (out); //johnfitz -- for per-surface frustum culling

	// lighting info

		for (i=0 ; i<BSP_MAX_LIGHTMAPS ; i++)
			out->styles[i] = in->bStyles[i];
		i = LittleLong(in->iLightOffset);
		if (i == -1)
			out->samples = NULL;
		else
			out->samples = loadmodel->lightdata+(i*3); //johnfitz -- lit support via lordhavoc (was "+ i")

		mMaterial = out->texinfo->texture->material;
		if (!mMaterial)
			Sys_Error("Failed to get a material for BSP surface! (%s)\n",out->texinfo->texture->name);
		
		//johnfitz -- this section rewritten
#ifdef _MSC_VER
#	pragma warning(suppress: 6011)
#endif
		if (!strncmp(mMaterial->cName, "sky3d", 5))
		{
			out->flags |= (SURF_DRAWTILED|SURFACE_SKIP);
			Mod_PolyForUnlitSurface(out);

			loadmodel->flags |= MODEL_FLAG_3DSKY;
		}
		else if(!strncmp(mMaterial->cName, "sky", 3)) // sky surface
		{
			out->flags |= (SURF_DRAWSKY | SURF_DRAWTILED);
			Mod_PolyForUnlitSurface(out); //no more subdivision
		}
		else if (mMaterial->flags & MATERIAL_FLAG_WATER) // warp surface
		{
#if 0
			out->flags |= (SURF_DRAWTURB | SURF_DRAWTILED);

			if ((!out->samples && SURF_DRAWTURB) || (out->samples && SURF_DRAWTILED))
				Mod_PolyForUnlitSurface(out);

			GL_SubdivideSurface (out);
#endif
		}
		else if (mMaterial->flags & MATERIAL_FLAG_MIRROR)
			out->flags |= SURFACE_MIRROR;
		else if(out->texinfo->flags & BSP_TEXTURE_MISSING) // texture is missing from bsp
		{
			if(out->samples) //lightmapped
				out->flags |= SURF_NOTEXTURE;
			else // not lightmapped
			{
				out->flags |= (SURF_NOTEXTURE | SURF_DRAWTILED);
				Mod_PolyForUnlitSurface (out);
			}
		}
		//johnfitz
	}
}

void Mod_SetParent (mnode_t *node, mnode_t *parent)
{
	node->parent = parent;
	if(node->contents < 0)
		return;

	Mod_SetParent(node->children[0],node);
	Mod_SetParent(node->children[1],node);
}

void Model_LoadBSPNodes(BSPLump_t *blLump)
{
	int				j;
	unsigned int	i, p, count;
	BSPNode_t		*in;
	mnode_t 		*out;

	in = (BSPNode_t *)(mod_base + blLump->iFileOffset);
	if (blLump->iFileLength % sizeof(*in))
		Sys_Error ("Model_LoadBSPNodes: funny lump size in %s",loadmodel->name);

	count	= blLump->iFileLength / sizeof(*in);
	out		= (mnode_t*)Hunk_AllocName ( count*sizeof(*out), loadname);

	loadmodel->nodes = out;
	loadmodel->numnodes = count;

	for (i = 0; i<count; i++, in++, out++)
	{
		for (j=0 ; j<3 ; j++)
		{
			out->minmaxs[j]		= LittleFloat(in->fMins[j]);
			out->minmaxs[3+j]	= LittleFloat(in->fMaxs[j]);
		}

		p = LittleLong(in->iPlaneNum);
		out->plane = loadmodel->planes + p;

		out->firstsurface	= LittleLong (in->usFirstFace); //johnfitz -- explicit cast as unsigned short
		out->numsurfaces	= LittleLong (in->usNumFaces); //johnfitz -- explicit cast as unsigned short

		for (j=0 ; j<2 ; j++)
		{
			//johnfitz -- hack to handle nodes > 32k, adapted from darkplaces
			p = (unsigned short)LittleLong(in->iChildren[j]);
			if (p < count)
				out->children[j] = loadmodel->nodes + p;
			else
			{
				p = 65535 - p; //note this uses 65535 intentionally, -1 is leaf 0
				if (p < loadmodel->numleafs)
					out->children[j] = (mnode_t *)(loadmodel->leafs + p);
				else
				{
					Con_Printf("Model_LoadBSPNodes: invalid leaf index %i (file has only %i leafs)\n", p, loadmodel->numleafs);
					out->children[j] = (mnode_t *)(loadmodel->leafs); //map it to the solid leaf
				}
			}
			//johnfitz
		}
	}

	Mod_SetParent (loadmodel->nodes, NULL);	// sets nodes and leafs
}

void Model_LoadBSPLeafs(BSPLump_t *blLump)
{
	BSPLeaf_t 	*in;
	mleaf_t 	*out;
	int			i, j, count, p;

	in = (BSPLeaf_t *)(mod_base + blLump->iFileOffset);
	if (blLump->iFileLength % sizeof(*in))
		Sys_Error ("Model_LoadBSPLeafs: funny lump size in %s",loadmodel->name);
	count = blLump->iFileLength / sizeof(*in);
	out = (mleaf_t*)Hunk_AllocName ( count*sizeof(*out), loadname);

	loadmodel->leafs	= out;
	loadmodel->numleafs = count;

	for ( i=0 ; i<count ; i++, in++, out++)
	{
		for (j=0 ; j<3 ; j++)
		{
			out->minmaxs[j]		= LittleFloat(in->fMins[j]);
			out->minmaxs[3+j]	= LittleFloat(in->fMaxs[j]);
		}

		p = LittleLong(in->iContents);
		out->contents = p;

		out->firstmarksurface	= loadmodel->marksurfaces+LittleLong(in->uiFirstMarkSurface);
		out->nummarksurfaces	= LittleLong(in->uiNumMarkSurfaces);

		p = LittleLong(in->iVisibilityOffset);
		if (p == -1)
			out->compressed_vis = NULL;
		else
			out->compressed_vis = loadmodel->visdata + p;
		out->efrags = NULL;

		for (j=0 ; j<4 ; j++)
			out->ambient_sound_level[j] = in->bAmbientLevel[j];

		//johnfitz -- removed code to mark surfaces as SURF_UNDERWATER
	}
}

void Model_LoadBSPClipNodes(BSPLump_t *blLump)
{
	BSPClipNode_t *in;
	BSPClipNode_t *out; //johnfitz -- was dclipnode_t
	int			i, count;
	hull_t		*hull;

	in = (BSPClipNode_t *)(mod_base + blLump->iFileOffset);
	if (blLump->iFileLength % sizeof(*in))
		Sys_Error ("Model_LoadBSPClipNodes: funny lump size in %s",loadmodel->name);
	count = blLump->iFileLength / sizeof(*in);
	out = (BSPClipNode_t*)Hunk_AllocName ( count*sizeof(*out), loadname);

	loadmodel->clipnodes = out;
	loadmodel->numclipnodes = count;

	for (i = 1; i < BSP_MAX_HULLS; i++)
	{
		hull = &loadmodel->hulls[i];
		hull->clipnodes = out;
		hull->firstclipnode = 0;
		hull->lastclipnode = count - 1;
		hull->planes = loadmodel->planes;

		Math_VectorCopy(vHullSizes[i][0], hull->clip_mins);
		Math_VectorCopy(vHullSizes[i][1], hull->clip_maxs);
	}

	for (i=0 ; i<count ; i++, out++, in++)
	{
		out->iPlaneNum = LittleLong(in->iPlaneNum);

		//johnfitz -- bounds check
		if (out->iPlaneNum < 0 || out->iPlaneNum >= loadmodel->numplanes)
			Host_Error ("Model_LoadBSPClipNodes: planenum out of bounds! (%i)",out->iPlaneNum);
		//johnfitz

		//johnfitz -- support clipnodes > 32k
		out->iChildren[0]	= LittleLong(in->iChildren[0]);
		out->iChildren[1]	= LittleLong(in->iChildren[1]);
		if (out->iChildren[0] >= count)
			out->iChildren[0] -= 65536;
		if (out->iChildren[1] >= count)
			out->iChildren[1] -= 65536;
		//johnfitz
	}
}

/*	Duplicate the drawing hull structure as a clipping hull
*/
void Mod_MakeHull0 (void)
{
	mnode_t			*in, *child;
	BSPClipNode_t	*out; //johnfitz -- was dclipnode_t
	int			i, j, count;
	hull_t		*hull;

	hull = &loadmodel->hulls[0];

	in = loadmodel->nodes;
	count = loadmodel->numnodes;
	out = (BSPClipNode_t*)Hunk_AllocName ( count*sizeof(*out), loadname);

	hull->clipnodes = out;
	hull->firstclipnode = 0;
	hull->lastclipnode = count-1;
	hull->planes = loadmodel->planes;

	for (i=0 ; i<count ; i++, out++, in++)
	{
		out->iPlaneNum = in->plane - loadmodel->planes;
		for (j=0 ; j<2 ; j++)
		{
			child = in->children[j];
			if (child->contents < 0)
				out->iChildren[j] = child->contents;
			else
				out->iChildren[j] = child - loadmodel->nodes;
		}
	}
}

void Model_LoadBSPMarkSurfaces(BSPLump_t *blLump)
{
	int		i, j, count;
	int		*in;
	msurface_t **out;

	in = (int *)(mod_base + blLump->iFileOffset);
	if (blLump->iFileLength % sizeof(*in))
		Sys_Error ("Model_LoadBSPMarkSurfaces: funny lump size in %s",loadmodel->name);
	count = blLump->iFileLength / sizeof(*in);
	out = (msurface_t**)Hunk_AllocName ( count*sizeof(*out), loadname);

	loadmodel->marksurfaces = out;
	loadmodel->nummarksurfaces = count;

	for ( i=0 ; i<count ; i++)
	{
		j = LittleLong(in[i]);
		if (j >= loadmodel->numsurfaces)
			Sys_Error ("Mod_ParseMarksurfaces: bad surface number");
		out[i] = loadmodel->surfaces + j;
	}
}

void Model_LoadBSPSurfaceEdges(BSPLump_t *blLump)
{
	int	i, count;
	int	*in, *out;

	in = (int *)(mod_base+blLump->iFileOffset);
	if(blLump->iFileLength%sizeof(*in))
		Sys_Error ("Model_LoadBSPSurfaceEdges: funny lump size in %s",loadmodel->name);

	count	= blLump->iFileLength/sizeof(*in);
	out		= (int*)Hunk_AllocName(count*sizeof(*out),loadname);

	loadmodel->surfedges	= out;
	loadmodel->numsurfedges = count;

	for ( i=0 ; i<count ; i++)
		out[i] = LittleLong (in[i]);
}

void Model_LoadBSPPlanes(BSPLump_t *blLump)
{
	int			i, j;
	mplane_t	*out;
	BSPPlane_t 	*in;
	int			count;
	int			bits;

	in = (BSPPlane_t *)(mod_base + blLump->iFileOffset);
	if (blLump->iFileLength % sizeof(*in))
		Sys_Error ("Model_LoadBSPPlanes: funny lump size in %s\n",loadmodel->name);
	count = blLump->iFileLength / sizeof(*in);
	out = (mplane_t*)Hunk_AllocName ( count*2*sizeof(*out), loadname);

	loadmodel->planes = out;
	loadmodel->numplanes = count;

	for ( i=0 ; i<count ; i++, in++, out++)
	{
		bits = 0;
		for (j=0 ; j<3 ; j++)
		{
			out->normal[j] = LittleFloat(in->fNormal[j]);
			if (out->normal[j] < 0)
				bits |= 1<<j;
		}

		out->dist		= LittleFloat(in->fDist);
		out->type		= LittleLong(in->iType);
		out->signbits	= bits;
	}
}

float RadiusFromBounds(plVector3f_t mins, plVector3f_t maxs)
{
	plVector3f_t	corner;

	for (int i=0 ; i<3 ; i++)
		corner[i] = fabs(mins[i]) > fabs(maxs[i]) ? fabs(mins[i]) : fabs(maxs[i]);

	return plLengthf(corner);
}

void Model_LoadBSPSubmodels(BSPLump_t *blLump)
{
	BSPModel_t	*in;
	BSPModel_t	*out;
	int			i, j, count;

	in = (BSPModel_t *)(mod_base+blLump->iFileOffset);
	if (blLump->iFileLength%sizeof(*in))
		Sys_Error ("Model_LoadBSPSubmodels: funny lump size in %s",loadmodel->name);

	count	= blLump->iFileLength/sizeof(*in);
	out		= (BSPModel_t*)Hunk_AllocName ( count*sizeof(*out), loadname);

	loadmodel->submodels	= out;
	loadmodel->numsubmodels = count;

	for(i = 0; i < count; i++,in++,out++)
	{
		for(j = 0; j < 3; j++)
		{	// spread the mins / maxs by a pixel
			out->fMins[j]	= LittleFloat(in->fMins[j])-1;
			out->fMaxs[j]	= LittleFloat(in->fMaxs[j])+1;
			out->fOrigin[j]	= LittleFloat(in->fOrigin[j]);
		}

		for (j=0 ; j<BSP_MAX_HULLS ; j++)
			out->iHeadNode[j] = LittleLong (in->iHeadNode[j]);

		out->iVisLeafs	= LittleLong(in->iVisLeafs);
		out->iFirstFace = LittleLong(in->iFirstFace);
		out->iNumFaces	= LittleLong(in->iNumFaces);
	}

	// johnfitz -- check world visleafs -- adapted from bjp
	out = loadmodel->submodels;

	if(out->iVisLeafs > BSP_MAX_LEAFS)
		Sys_Error ("Model_LoadBSPSubmodels: too many visleafs (%d, max = %d) in %s", out->iVisLeafs, BSP_MAX_LEAFS, loadmodel->name);
	//johnfitz
}

/*	Update the model's clipmins and clipmaxs based on each node's plane.

	This works because of the way brushes are expanded in hull generation.
	Each brush will include all six axial planes, which bound that brush.
	Therefore, the bounding box of the hull can be constructed entirely
	from axial planes found in the clipnodes for that hull.
*/
void Mod_BoundsFromClipNode (model_t *mod, int hull, int nodenum)
{
	mplane_t		*plane;
	BSPClipNode_t	*node;

	if (nodenum < 0)
		return; //hit a leafnode

	node = &mod->clipnodes[nodenum];
	plane = mod->hulls[hull].planes + node->iPlaneNum;
	switch (plane->type)
	{

	case PLANE_X:
		if (plane->signbits == 1)
			mod->clipmins[0] = Math_Min (mod->clipmins[0], -plane->dist - mod->hulls[hull].clip_mins[0]);
		else
			mod->clipmaxs[0] = Math_Max (mod->clipmaxs[0], plane->dist - mod->hulls[hull].clip_maxs[0]);
		break;
	case PLANE_Y:
		if (plane->signbits == 2)
			mod->clipmins[1] = Math_Min (mod->clipmins[1], -plane->dist - mod->hulls[hull].clip_mins[1]);
		else
			mod->clipmaxs[1] = Math_Max (mod->clipmaxs[1], plane->dist - mod->hulls[hull].clip_maxs[1]);
		break;
	case PLANE_Z:
		if (plane->signbits == 4)
			mod->clipmins[2] = Math_Min (mod->clipmins[2], -plane->dist - mod->hulls[hull].clip_mins[2]);
		else
			mod->clipmaxs[2] = Math_Max (mod->clipmaxs[2], plane->dist - mod->hulls[hull].clip_maxs[2]);
		break;
	default:
		//skip nonaxial planes; don't need them
		break;
	}

	Mod_BoundsFromClipNode (mod, hull, node->iChildren[0]);
	Mod_BoundsFromClipNode (mod, hull, node->iChildren[1]);
}

void Model_LoadBSP(model_t *mod,void *buffer)
{
	int				version;
	unsigned int	i, j;
	BSPHeader_t		*bhHeader;
	BSPModel_t 		*bm;
	float			radius; //johnfitz

	bhHeader = (BSPHeader_t*)buffer;

	version = LittleLong(bhHeader->iVersion);
	if(version != BSP_VERSION)
		Console_ErrorMessage(false,mod->name,va("Wrong version (%i should be %i)",version,BSP_VERSION));

	loadmodel->version	= version;
	loadmodel->type		= MODEL_TYPE_LEVEL;

	// swap all the lumps
	mod_base = (uint8_t*)bhHeader;

	for (i=0; i < sizeof(BSPHeader_t)/4; i++)
		((int*)bhHeader)[i] = LittleLong(((int*)bhHeader)[i]);

	// Load into heap
	Model_LoadBSPVertexes(&bhHeader->bLumps[LUMP_VERTEXES]);
	Model_LoadBSPEdges(&bhHeader->bLumps[LUMP_EDGES]);
	Model_LoadBSPSurfaceEdges(&bhHeader->bLumps[LUMP_SURFEDGES]);
	Model_LoadBSPTextures(&bhHeader->bLumps[LUMP_TEXTURES]);
	Model_LoadBSPLighting(&bhHeader->bLumps[LUMP_LIGHTING]);
	Model_LoadBSPPlanes(&bhHeader->bLumps[LUMP_PLANES]);
	Model_LoadBSPTextureInfo(&bhHeader->bLumps[LUMP_TEXINFO]);
	Model_LoadBSPFaces(&bhHeader->bLumps[LUMP_FACES]);
	Model_LoadBSPMarkSurfaces(&bhHeader->bLumps[LUMP_MARKSURFACES]);
	Model_LoadBSPVisibility(&bhHeader->bLumps[LUMP_VISIBILITY]);
	Model_LoadBSPLeafs(&bhHeader->bLumps[LUMP_LEAFS]);
	Model_LoadBSPNodes(&bhHeader->bLumps[LUMP_NODES]);
	Model_LoadBSPClipNodes(&bhHeader->bLumps[LUMP_CLIPNODES]);
	Model_LoadBSPEntities(&bhHeader->bLumps[LUMP_ENTITIES]);
	Model_LoadBSPSubmodels(&bhHeader->bLumps[LUMP_MODELS]);

	Mod_MakeHull0();

	mod->numframes = 2;		// regular and alternate animation

//
// set up the submodels (FIXME: this is confusing)
//
	// johnfitz -- okay, so that i stop getting confused every time i look at this loop, here's how it works:
	// we're looping through the submodels starting at 0.  Submodel 0 is the main model, so we don't have to
	// worry about clobbering data the first time through, since it's the same data.  At the end of the loop,
	// we create a new copy of the data to use the next time through.
	for (i=0 ; i<mod->numsubmodels ; i++)
	{
		bm = &mod->submodels[i];

		mod->hulls[0].firstclipnode = bm->iHeadNode[0];
		for (j=1 ; j<BSP_MAX_HULLS ; j++)
		{
			mod->hulls[j].firstclipnode = bm->iHeadNode[j];
			mod->hulls[j].lastclipnode	= mod->numclipnodes-1;
		}

		mod->firstmodelsurface	= bm->iFirstFace;
		mod->nummodelsurfaces	= bm->iNumFaces;

		Math_VectorCopy (bm->fMaxs, mod->maxs);
		Math_VectorCopy (bm->fMins, mod->mins);

		//johnfitz -- calculate rotate bounds and yaw bounds
		radius = RadiusFromBounds (mod->mins, mod->maxs);
		mod->rmaxs[0] = mod->rmaxs[1] = mod->rmaxs[2] = mod->ymaxs[0] = mod->ymaxs[1] = mod->ymaxs[2] = radius;
		mod->rmins[0] = mod->rmins[1] = mod->rmins[2] = mod->ymins[0] = mod->ymins[1] = mod->ymins[2] = -radius;
		//johnfitz

		//johnfitz -- correct physics cullboxes so that outlying clip brushes on doors and stuff are handled right
		if (i > 0 || strcmp(mod->name, sv.modelname) != 0) //skip submodel 0 of sv.worldmodel, which is the actual world
		{
			// start with the hull0 bounds
			Math_VectorCopy (mod->maxs, mod->clipmaxs);
			Math_VectorCopy (mod->mins, mod->clipmins);
		}
		//johnfitz

		mod->numleafs = bm->iVisLeafs;

		if (i < mod->numsubmodels-1)
		{
			// duplicate the basic information
			char	name[10];

			sprintf (name, "*%i", i+1);
			loadmodel = Model_FindName(name);
			*loadmodel = *mod;
			strncpy(loadmodel->name, name, sizeof(loadmodel->name));
			mod = loadmodel;
		}
	}
}

MathVector_t Model_GenerateNormal3fv(MathVector3f_t a, MathVector3f_t b, MathVector3f_t c)
{
	MathVector_t	output;
	MathVector3f_t	normal, x, y;

	Math_VectorSubtract(c, b, x);
	Math_VectorSubtract(a, b, y);

	plVectorClear(normal);
	Math_CrossProduct(x, y, normal);

	// Normalize it.
	plVectorNormalize(normal);

	// Return the normal.
	Math_VectorToMV(normal, output);

	return output;
}

MathVector_t Model_GenerateNormal3f(
	float aX, float aY, float aZ,
	float bX, float bY, float bZ,
	float cX, float cY, float cZ)
{
	MathVector3f_t a, b, c;

#if 0
	Con_Printf("A : %f %f %f\n", aX, aY, aZ);
	Con_Printf("B : %f %f %f\n", bX, bY, bZ);
	Con_Printf("C : %f %f %f\n", cX, cY, cZ);
#endif

	a[0] = aX; a[1] = aY; a[2] = aZ;
	b[0] = bX; b[1] = bY; b[2] = bZ;
	c[0] = cX; c[1] = cY; c[2] = cZ;

	return Model_GenerateNormal3fv(a, b, c);
}

void Model_LoadRelativeMaterial(model_t *model)
{
	char out[PLATFORM_MAX_PATH];

	plStripExtension(out, model->name);

	model->materials = Material_Load(out);
	if (!model->materials)
	{
		Con_Warning("Failed to load material for model! (%s) (%s)\n", model->name, out);

		// Set us up to just use the dummy material instead.
		model->materials = g_mMissingMaterial;
	}
}

/*
	MD2 Models
*/

/*	Calculate bounds of alias model for nonrotated, yawrotated, and fullrotated cases
*/
void Model_CalculateMD2Bounds(model_t *model, MD2_t *alias_model)
{
	// Reset everything to its maximum size.
	for (int i = 0; i < 3; i++)
	{
		model->mins[i] = model->ymins[i] = model->rmins[i] = 999999.0f;
		model->maxs[i] = model->ymaxs[i] = model->rmaxs[i] = -999999.0f;
	}

	MD2Frame_t *curframe = (MD2Frame_t*)((uint8_t*)alias_model + alias_model->ofs_frames + alias_model->framesize);
	if (!curframe)
	{
		Con_Warning("Invalid frame encountered when calculating MD2 bounds! (%s)\n", model->name);
		return;
	}

	MathVector3f_t mins = { 0 }, maxs = { 0 }, curmins, curmaxs;
	MD2TriangleVertex_t *vertices = &curframe->verts[0];
	for (int i = 0; i < alias_model->num_xyz; i++, vertices++)
	{
		for (int j = 0; j < 3; j++)
		{
			curmins[j] = -(vertices->v[j] + curframe->translate[j]);
			if (curmins[j] < mins[j])
				mins[j] = curmins[j];

			curmaxs[j] = (vertices->v[j] + curframe->translate[j]);
			if (curmaxs[j] > maxs[j])
				maxs[j] = curmaxs[j];
		}
	}

	// Check that the size is valid.
	if (plVectorCompare(mins, pl_origin3f) && plVectorCompare(maxs, pl_origin3f))
	{
		// This should never happen, but if it does, give a warning.
		Con_Warning("Suspicious bounding box size! (%s)\n", model->name);

		// Then give us a default size.
		Math_VectorSet(-32, mins);
		Math_VectorSet(32, maxs);
	}

	Math_VectorCopy(mins, model->mins);
	Math_VectorCopy(maxs, model->maxs);

	Math_VectorCopy(model->mins, model->rmins);
	Math_VectorCopy(model->maxs, model->rmaxs);
	Math_VectorCopy(model->mins, model->ymins);
	Math_VectorCopy(model->maxs, model->ymaxs);
}

/*	Calculate the normals for MD2 models.
*/
void Model_CalculateMD2Normals(model_t *model, MD2_t *md2)
{
#if 0
	MD2Frame_t *frame;
	MD2Triangle_t *triangles;
	MD2TriangleVertex_t *vertices;
	int i; //, j, v;

	frame = (MD2Frame_t*)((uint8_t*)md2 + md2->ofs_frames + md2->framesize);

	vertices = &frame->verts[0];

	triangles = (MD2Triangle_t*)((uint8_t*)md2 + md2->ofs_tris);
	for (i = 0; i < md2->numtris; i++, triangles++)
	{
		MathVector_t normalVector = Model_GenerateNormal3f(
			vertices[triangles[i].index_xyz[0]].v[0] * frame->scale[0] + frame->translate[0],
			vertices[triangles[i].index_xyz[0]].v[1] * frame->scale[1] + frame->translate[1],
			vertices[triangles[i].index_xyz[0]].v[2] * frame->scale[2] + frame->translate[2],

			vertices[triangles[i].index_xyz[1]].v[0] * frame->scale[0] + frame->translate[0],
			vertices[triangles[i].index_xyz[1]].v[1] * frame->scale[1] + frame->translate[1],
			vertices[triangles[i].index_xyz[1]].v[2] * frame->scale[2] + frame->translate[2],

			vertices[triangles[i].index_xyz[2]].v[0] * frame->scale[0] + frame->translate[0],
			vertices[triangles[i].index_xyz[2]].v[1] * frame->scale[1] + frame->translate[1],
			vertices[triangles[i].index_xyz[2]].v[2] * frame->scale[2] + frame->translate[2]);

		// X Y Z
		MathVector3f_t normal;
		Math_MVToVector(normalVector, normal);
		Math_VectorCopy(normal, model->object.ovVertices[v].mvNormal);
	}
#endif
}

void Model_LoadMD2(model_t *mModel,void *Buffer)
{
	int				i, j,
					iVersion,
					numframes,
					*pinglcmd,*poutglcmd,
					iStartHunk,iEnd,total;
	MD2_t			*pinmodel,*mMD2Model;
	MD2Triangle_t	*pintriangles, *pouttriangles;
	MD2Frame_t		*pinframe, *poutframe;

	iStartHunk = Hunk_LowMark();

	pinmodel = (MD2_t*)Buffer;

	iVersion = LittleLong(pinmodel->version);
	if (iVersion != MD2_VERSION)
	{
		Con_Error("%s has wrong version number (%i should be %i)\n", mModel->name, iVersion, MD2_VERSION);
		return;
	}

	mMD2Model = (MD2_t*)Hunk_AllocName(LittleLong(pinmodel->ofs_end) + sizeof(MD2_t), loadname);
	for(i = 0; i < 17; i++)
		((int*)mMD2Model)[i] = LittleLong(((int*)pinmodel)[i]);

	mModel->type		= MODEL_TYPE_MD2;
	mModel->version		= iVersion;
	mModel->flags		= 0;
	mModel->numframes	= numframes = mMD2Model->num_frames;

	if(mMD2Model->ofs_skins <= 0 || mMD2Model->ofs_skins >= mMD2Model->ofs_end)
		Sys_Error("%s is not a valid model",mModel->name);
	else if(mMD2Model->ofs_st <= 0 || mMD2Model->ofs_st >= mMD2Model->ofs_end)
		Sys_Error("%s is not a valid model",mModel->name);
	else if(mMD2Model->ofs_tris <= 0 || mMD2Model->ofs_tris >= mMD2Model->ofs_end)
		Sys_Error("%s is not a valid model",mModel->name);
	else if(mMD2Model->ofs_frames <= 0 || mMD2Model->ofs_frames >= mMD2Model->ofs_end)
		Sys_Error("%s is not a valid model",mModel->name);
	else if(mMD2Model->ofs_glcmds <= 0 || mMD2Model->ofs_glcmds >= mMD2Model->ofs_end)
		Sys_Error("%s is not a valid model",mModel->name);
	else if(mMD2Model->numtris < 1 || mMD2Model->numtris > MD2_MAX_TRIANGLES)
		Sys_Error("%s has invalid number of triangles (%i)",mModel->name,mMD2Model->numtris);
	else if(mMD2Model->num_xyz < 1 || mMD2Model->num_xyz > MD2_MAX_VERTICES)
		Sys_Error("%s has invalid number of vertices (%i)",mModel->name,mMD2Model->num_xyz);
	else if(mMD2Model->num_frames < 1 || mMD2Model->num_frames > MD2_MAX_FRAMES)
		Sys_Error("%s has invalid number of frames (%i)",mModel->name,mMD2Model->num_frames);
	else if(mMD2Model->num_skins < 0 || mMD2Model->num_skins > MD2_MAX_SKINS)
		Sys_Error("%s has invalid number of skins (%i)",mModel->name,mMD2Model->num_skins);

	for(i = 0; i < 7; i++)
		((int*)&mMD2Model->ofs_skins)[i] += sizeof(mMD2Model);

	pintriangles = (MD2Triangle_t*)((uint8_t*)pinmodel+LittleLong(pinmodel->ofs_tris));
	pouttriangles = (MD2Triangle_t*)((uint8_t*)mMD2Model+mMD2Model->ofs_tris);
	for(i=0; i < mMD2Model->numtris; i++)
	{
		for(j=0; j < 3; j++)
		{
			pouttriangles->index_xyz[j] = LittleShort(pintriangles->index_xyz[j]);
			pouttriangles->index_st[j]	= LittleShort(pintriangles->index_st[j]);

			if(pouttriangles->index_xyz[j] >= mMD2Model->num_xyz || pouttriangles->index_st[j] >= mMD2Model->num_st)
				Sys_Error("Model has invalid vertex indices! (%s) (%i)",mModel->name,(int)pouttriangles->index_xyz[j]);
		}

		pintriangles++;
		pouttriangles++;
	}

	pinframe = (MD2Frame_t*)((uint8_t*)pinmodel + LittleLong(pinmodel->ofs_frames));
	poutframe = (MD2Frame_t*)((uint8_t*)mMD2Model + mMD2Model->ofs_frames);
	for (i = 0; i < numframes; i++)
	{
		for (j = 0; j < 3; j++)
		{
			poutframe->scale[j] = LittleFloat(pinframe->scale[j]);
			poutframe->translate[j] = LittleFloat(pinframe->translate[j]);
		}

		for (j = 0; j < 16; j++)
			poutframe->name[j] = pinframe->name[j];

		for (j = 0; j < mMD2Model->num_xyz; j++)
		{
			poutframe->verts[j].v[0] = pinframe->verts[j].v[0];
			poutframe->verts[j].v[1] = pinframe->verts[j].v[1];
			poutframe->verts[j].v[2] = pinframe->verts[j].v[2];
			poutframe->verts[j].lightnormalindex = pinframe->verts[j].lightnormalindex;
		}

		pinframe = (MD2Frame_t*)&pinframe->verts[j].v[0];
		poutframe = (MD2Frame_t*)&poutframe->verts[j].v[0];
	}

	pinglcmd = (int*)((uint8_t*)pinmodel+LittleLong(pinmodel->ofs_glcmds));
	poutglcmd = (int*)((uint8_t*)mMD2Model+mMD2Model->ofs_glcmds);
	for(i=0; i < mMD2Model->num_glcmds; i++)
		*poutglcmd++ = LittleLong(*pinglcmd++);

	memcpy
	(
		(char*)mMD2Model+mMD2Model->ofs_skins,
		(char*)pinmodel+mMD2Model->ofs_skins,
		mMD2Model->num_skins*MAX_QPATH
	);

#if 0
	// Allocate vertex array.
	mModel->object.numverts = mMD2Model->numtris * 3;
	mModel->object.vertices = (vlVertex_t*)malloc(mModel->object.numverts * sizeof(vlVertex_t));
	memset(mModel->object.vertices, 0, mModel->object.numverts * sizeof(vlVertex_t));
#endif

	Model_LoadRelativeMaterial(mModel);

	// Process the appropriate model bounds.
	Model_CalculateMD2Bounds(mModel, mMD2Model);

	// Calculate vertex normals.
	Model_CalculateMD2Normals(mModel, mMD2Model);

	iEnd = Hunk_LowMark();
	total = iEnd-iStartHunk;

	Cache_Alloc(&mModel->cache,total,loadname);
	if(!mModel->cache.data)
		return;

	memcpy(mModel->cache.data, mMD2Model, total);

	Hunk_FreeToLowMark(iStartHunk);
}

/*
	OBJ Support
*/

bool Model_LoadOBJ(model_t *model)
{
	char fullp[PL_MAX_PATH];
	sprintf(fullp, "%s/%s", host_parms.basepath, model->name);
	
	plStaticModel_t *obj_model = plLoadOBJModel(fullp);
	if (!obj_model)
	{
		Con_Warning("Failed to load OBJ model! (%s)\n", model->name);
		return false;
	}

	model->type	= MODEL_TYPE_STATIC;

	return true;
}

/**/

void Model_PrintMemoryCache(void)
{
	int		i;
	model_t	*mod;

	Con_SafePrintf("Cached models:\n"); //johnfitz -- safeprint instead of print
	for (i=0, mod=mod_known ; i < mod_numknown ; i++, mod++)
		Con_SafePrintf ("%8p : %s\n", mod->cache.data, mod->name); //johnfitz -- safeprint instead of print

	Con_Printf ("%i models\n",mod_numknown); //johnfitz -- print the total too
}


