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

#ifndef BSPFILE_H
#define BSPFILE_H

#include "SharedFormats.h"

#define	MAX_KEY		32
#define	MAX_VALUE	1024

typedef struct
{
	int			numhulls,
				filehulls;

	float		hullsizes[BSP_MAX_HULLS][2][3];
} hullinfo_t;

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5

//============================================================================

// the utilities get to be lazy and just use large static arrays

extern	unsigned int		nummodels;
extern	BSPModel_t			dmodels[BSP_MAX_MODELS];
extern	int					visdatasize;
extern	byte				dvisdata[BSP_MAX_VISIBILITY];
extern	int					lightdatasize;
extern	byte				dlightdata[BSP_MAX_LIGHTING];
extern	int					rgblightdatasize;
extern	byte				drgblightdata[BSP_MAX_LIGHTING*3];
extern	int					texdatasize;
extern	byte				dtexdata[BSP_MAX_MIPTEX]; // (dmiptexlump_t)
extern	int					entdatasize;
extern	char				dentdata[BSP_MAX_ENTSTRING];
extern	unsigned int		numleafs;
extern	BSPLeaf_t			dleafs[BSP_MAX_LEAFS];
extern	unsigned int		numplanes;
extern	BSPPlane_t			dplanes[BSP_MAX_PLANES];
extern	unsigned int		numvertexes;
extern	BSPVertex_t			dvertexes[BSP_MAX_VERTS];
extern	unsigned int		numnodes;
extern	BSPNode_t			dnodes[BSP_MAX_NODES];
extern	unsigned int		numtexinfo;
extern	BSPTextureInfo_t	texinfo[BSP_MAX_TEXINFO];
extern	unsigned int		numfaces;
extern	BSPFace_t			dfaces[BSP_MAX_FACES];
extern	int					numclipnodes;
extern	BSPClipNode_t		dclipnodes[BSP_MAX_CLIPNODES];
extern	unsigned int		numedges;
extern	BSPEdge_t			dedges[BSP_MAX_EDGES];
extern	unsigned int		nummarksurfaces;
extern	unsigned int		dmarksurfaces[BSP_MAX_MARKSURFACES];
extern	unsigned int		numsurfedges;
extern	int					dsurfedges[BSP_MAX_SURFEDGES];

extern	hullinfo_t	hullinfo;

typedef struct epair_s
{
	struct epair_s	*next;
	char	*key;
	char	*value;
} epair_t;

typedef struct
{
	epair_t		*epairs;
	struct mbrush_s *brushes;
} entity_t;

extern	int			num_entities;
extern	entity_t	entities[BSP_MAX_ENTITIES];

void	PrintEntity (entity_t *ent);
char 	*ValueForKey (entity_t *ent, char *key);
vec_t	FloatForKey (entity_t *ent, char *key);
void	SetKeyValue (entity_t *ent, char *key, char *value);
entity_t *FindEntityWithKeyPair( char *key, char *value );
void 	GetVectorForKey (entity_t *ent, char *key, vec3_t vec);
epair_t *ParseEpair (void);
void	ParseEntities (void);
void	UnparseEntities (void);

int		CompressVis (byte *vis, byte *dest, int visrow);
void	DecompressVis(byte *in, byte *out, int size);

void LoadBSPFile(char *filename);
void WriteBSPFile(char *filename);
void PrintBSPFileSizes(void);

#endif
