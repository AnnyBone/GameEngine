/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others

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

#ifndef __ENGINEMODEL__
#define __ENGINEMODEL__

#include "shared_formats.h"

/*
d*_t structures are on-disk representations
m*_t structures are in-memory
*/


/*
==============================================================================

BRUSH MODELS

==============================================================================
*/

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2


// plane_t structure
// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct mplane_s
{
	vec3_t	normal;
	float	dist;
	byte	type;			// for texture axis selection and fast side tests
	byte	signbits;		// signx + signy<<1 + signz<<1
	byte	pad[2];
} mplane_t;

typedef struct texture_s
{
	char				name[16];
	unsigned			width, height;
	struct gltexture_s	*gltexture; //johnfitz -- pointer to gltexture

	int	iAssignedMaterial;	// HACKY!!!!!

	struct msurface_s	*texturechain;	// for texture chains
	int					anim_total;				// total tenths in sequence ( 0 = no)
	int					anim_min, anim_max;		// time for this frame min <=time< max
	struct texture_s	*anim_next;		// in the animation sequence
	struct texture_s	*alternate_anims;	// bmodels in frmae 1 use these
	unsigned			offsets[BSP_MIP_LEVELS];		// four mip maps stored
} texture_t;

#define	SURF_PLANEBACK		2
#define	SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40
#define SURF_UNDERWATER		0x80
#define SURF_NOTEXTURE		0x100 //johnfitz

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
	unsigned int	v[2];
	unsigned int	cachededgeoffset;
} medge_t;

typedef struct
{
	float		vecs[2][4];

	texture_t	*texture;

	int			flags;
} mtexinfo_t;

#define	VERTEXSIZE	7

typedef struct glpoly_s
{
	struct	glpoly_s	*next;
	struct	glpoly_s	*chain;
	int		numverts;
	float	verts[4][VERTEXSIZE];	// variable sized (xyz s1t1 s2t2)
} glpoly_t;

typedef struct msurface_s
{
	int			visframe;		// should be drawn when node is crossed
	bool		culled;			// johnfitz -- for frustum culling
	float		mins[3];		// johnfitz -- for frustum culling
	float		maxs[3];		// johnfitz -- for frustum culling

	mplane_t	*plane;
	int			flags;

	int			firstedge;	// look up in model->surfedges[], negative numbers
	int			numedges;	// are backwards edges

	short		texturemins[2];
	short		extents[2];

	int			light_s, light_t;	// gl lightmap coordinates

	glpoly_t	*polys;				// multiple if warped
	struct	msurface_s	*texturechain;

	mtexinfo_t	*texinfo;

// lighting info
	int			dlightframe,dlightbits;

	int			lightmaptexturenum;
	byte		styles[BSP_MAX_LIGHTMAPS];
	int			cached_light[BSP_MAX_LIGHTMAPS];	// values currently used in lightmap
	bool		cached_dlight;				// TRUE if dynamic light in cache
	byte		*samples;		// [numstyles*surfsize]
} msurface_t;

typedef struct mnode_s
{
// common with leaf
	int			contents;		// 0, to differentiate from leafs
	int			visframe;		// node needs to be traversed if current

	float		minmaxs[6];		// for bounding box culling

	struct mnode_s	*parent;

// node specific
	mplane_t		*plane;
	struct mnode_s	*children[2];

	unsigned int	firstsurface,
					numsurfaces;
} mnode_t;

typedef struct mleaf_s
{
// common with node
	int			contents;		// wil be a negative contents number
	int			visframe;		// node needs to be traversed if current

	float		minmaxs[6];		// for bounding box culling

	struct mnode_s	*parent;

// leaf specific
	byte		*compressed_vis;
	efrag_t		*efrags;

	msurface_t	**firstmarksurface;
	unsigned int	nummarksurfaces;
	int			key;			// BSP sequence number for leaf's contents
	byte		ambient_sound_level[BSP_AMBIENT_END];
} mleaf_t;

// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct
{
	BSPClipNode_t	*clipnodes; //johnfitz -- was dclipnode_t

	mplane_t		*planes;

	int				firstclipnode;
	int				lastclipnode;

	vec3_t			clip_mins;
	vec3_t			clip_maxs;
} hull_t;

//
// Whole model
//

#define	MODEL_MAX_TEXTURES	32

typedef struct model_s
{
	char		name[MAX_QPATH];

	bool		bNeedLoad;		// bmodels and sprites don't cache normally

	ModelType_t	mType;

	int			version;
	int			numframes;

	int			flags;

	// volume occupied by the model graphics
	vec3_t		mins, maxs;
	vec3_t		ymins, ymaxs; //johnfitz -- bounds for entities with nonzero yaw
	vec3_t		rmins, rmaxs; //johnfitz -- bounds for entities with nonzero pitch or roll
	//johnfitz -- removed float radius;

	// solid volume for clipping
	bool		clipbox;
	vec3_t		clipmins, clipmaxs;

	// brush model
	unsigned int	firstmodelsurface, nummodelsurfaces;

	int			numsubmodels;
	BSPModel_t	*submodels;

	int			numplanes;
	mplane_t	*planes;

	int			numleafs;		// number of visible leafs, not counting 0
	mleaf_t		*leafs;

	int			numvertexes;
	BSPVertex_t	*vertexes;

	int			numedges;
	medge_t		*edges;

	int			numnodes;
	mnode_t		*nodes;

	int			numtexinfo;
	mtexinfo_t	*texinfo;

	int			numsurfaces;
	msurface_t	*surfaces;

	int				numsurfedges;
	int				*surfedges;

	int				numclipnodes;
	BSPClipNode_t	*clipnodes; //johnfitz -- was dclipnode_t

	int				nummarksurfaces;
	msurface_t		**marksurfaces;

	hull_t			hulls[BSP_MAX_HULLS];

	int				numtextures;
	texture_t		**textures;

	byte			*visdata,*lightdata;
	char			*entities;

	// additional model data
	cache_user_t	cache;		// only access through Mod_Extradat

	// Material System
	int	iAssignedMaterials;
} model_t;

//============================================================================

void	Model_Initialize(void);
void	Model_ClearAll(void);
model_t *Mod_ForName(char *cName);
void	*Mod_Extradata (model_t *mod);	// handles caching
void	Model_Touch(char *cName);

mleaf_t *Mod_PointInLeaf (float *p, model_t *model);
byte	*Mod_LeafPVS (mleaf_t *leaf, model_t *model);

// OBJ Support
void Model_DrawOBJ(entity_t *eEntity);

#endif	// __MODEL__
