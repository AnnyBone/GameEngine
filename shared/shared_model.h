/*	Copyright (C) 2011-2016 OldTimes Software

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

#pragma once

#include "shared_formats.h"
#include "shared_zone.h"

#define	MODEL_FLAG_3DSKY	(1 << 0)	// Let's us know that the model supports 3D skies.

typedef struct efrag_s
{
	struct mleaf_s		*leaf;
	struct efrag_s		*leafnext;
	struct ClientEntity_s *entity;
	struct efrag_s		*entnext;
} efrag_t;

// plane_t structure
// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct mplane_s
{
	PLVector3D		normal;
	float			dist;
	uint8_t			type;			// for texture axis selection and fast side tests
	uint8_t			signbits;		// signx + signy<<1 + signz<<1
	uint8_t			pad[2];
} mplane_t;

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

typedef struct texture_s
{
	char				name[16];
	unsigned			width, height;
	struct gltexture_s	*gltexture; //johnfitz -- pointer to gltexture

	struct Material_s	*material;

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
#define SURFACE_MIRROR		0x40
#define SURF_UNDERWATER		0x80
#define SURF_NOTEXTURE		0x100 //johnfitz
#define	SURFACE_SKIP		0x200

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
	int			dlightframe, dlightbits;

	int			lightmaptexturenum;
	uint8_t		styles[BSP_MAX_LIGHTMAPS];
	int			cached_light[BSP_MAX_LIGHTMAPS];	// values currently used in lightmap
	bool		cached_dlight;				// TRUE if dynamic light in cache
	uint8_t		*samples;		// [numstyles*surfsize]
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
	PLbyte		*compressed_vis;
	efrag_t		*efrags;

	msurface_t	**firstmarksurface;
	unsigned int	nummarksurfaces;
	int			key;			// BSP sequence number for leaf's contents
	PLbyte		ambient_sound_level[BSP_AMBIENT_END];
} mleaf_t;

// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct
{
	BSPClipNode_t	*clipnodes; //johnfitz -- was dclipnode_t

	mplane_t		*planes;

	int				firstclipnode;
	int				lastclipnode;

	PLVector3D	clip_mins;
	PLVector3D	clip_maxs;
} hull_t;

#include "platform_graphics.h"

typedef struct model_s
{
	char		name[PL_SYSTEM_MAX_PATH];

	bool		bNeedLoad;		// bmodels and sprites don't cache normally

	ModelType_t	type;

	int			version;

	int			flags;

	// volume occupied by the model graphics
	PLVector3D	mins, maxs;
	PLVector3D	ymins, ymaxs; //johnfitz -- bounds for entities with nonzero yaw
	PLVector3D	rmins, rmaxs; //johnfitz -- bounds for entities with nonzero pitch or roll
	//johnfitz -- removed float radius;

	// solid volume for clipping
	bool			clipbox;
	PLVector3D	clipmins, clipmaxs;

	// brush model
	unsigned int	firstmodelsurface, nummodelsurfaces;

	unsigned int	numsubmodels;
	BSPModel_t		*submodels;

	int			numplanes;
	mplane_t	*planes;

	unsigned int	numleafs;		// number of visible leafs, not counting 0
	mleaf_t			*leafs;

	int			numvertexes;
	BSPVertex_t	*vertexes;

	int			numedges;
	medge_t		*edges;

	unsigned int	numnodes;
	mnode_t			*nodes;

	int			numtexinfo;
	mtexinfo_t	*texinfo;

	int			numsurfaces;
	msurface_t	*surfaces;

	int				numsurfedges;
	int				*surfedges;

	unsigned int	numclipnodes;
	BSPClipNode_t	*clipnodes; //johnfitz -- was dclipnode_t

	int				nummarksurfaces;
	msurface_t		**marksurfaces;

	hull_t			hulls[BSP_MAX_HULLS];

	unsigned int	numtextures;
	texture_t		**textures;

	uint8_t			*visdata, *lightdata;
	char			*entities;

	// additional model data
	cache_user_t	cache;		// only access through Mod_Extradat

	// Material System
	struct Material_s *materials;

	PLMesh			**objects;
	unsigned int	numtriangles;	// Static counter for triangles.
	int				numframes;		// Number of objects / frames.
} model_t;