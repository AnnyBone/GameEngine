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

/*
	BSP Format
*/

#define	BSP_HEADER			(('4'<<24)+('L'<<16)+('V'<<8)+'L')	// For easy identification.
#define	BSP_HEADER_SIZE		8									// "BSP" followed by version number.

#define	BSP_VERSION		5
#define	BSP_EXTENSION	".level"

#define	BSP_MIP_LEVELS	4

enum
{
	BSP_HULL_EMPTY,
	BSP_HULL_STAND,
	BSP_HULL_LARGE,
	BSP_HULL_CROUCH,

	BSP_HULL_MAX
};

#ifdef SHARED_FORMATS_C
	extern float vHullSizes[BSP_HULL_MAX][2][3];
#endif

#define	BSP_MAX_HULLS			BSP_HULL_MAX
#define	BSP_MAX_LEAFS			32768			//0x400000
#define	BSP_MAX_ENTITIES		32768
#define	BSP_MAX_VERTS			65535			//0x100000
#define	BSP_MAX_MODELS			4096
#define	BSP_MAX_BRUSHES			32768			//0x100000
#define	BSP_MAX_ENTSTRING		0x400000
#define	BSP_MAX_PLANES			65536			//0x200000
#define	BSP_MAX_NODES			32767			//0x200000
#define	BSP_MAX_CLIPNODES		32767			//0x800000
#define	BSP_MAX_FACES			65536			//0x200000
#define	BSP_MAX_MARKSURFACES	0x400000
#define	BSP_MAX_TEXINFO			BSP_MAX_FACES	//0x100000
#define	BSP_MAX_EDGES			0x400000
#define	BSP_MAX_SURFEDGES		0x200000		//0x800000
#define	BSP_MAX_MIPTEX			0x800000		//0x1000000
#define	BSP_MAX_LIGHTING		0x400000		//0x1000000
#define	BSP_MAX_LIGHTMAPS		4
#define	BSP_MAX_VISIBILITY		0x400000		//0x1000000

#define	BSP_TEXTURE_SKY			(1 << 0)
#define	BSP_TEXTURE_WATER		(1 << 1)
#define	BSP_TEXTURE_SKIP		(1 << 2)	// Surface to be skipped during rendering (and lighting).
#define	BSP_TEXTURE_MISSING		(1 << 3)	// Indicates that the texture is missing.

/*	Different content types
	set for brushes by Katana Level.
	Probably best not to touch
	unless you're planning on
	updating the tools yourself.
*/
#define	BSP_CONTENTS_EMPTY	-1	// Empty space.
#define	BSP_CONTENTS_SOLID	-2	// Solid brush.
#define	BSP_CONTENTS_WATER	-3	// Water contents.
#define BSP_CONTENTS_SLIME  -4	// Slime contents.
#define BSP_CONTENTS_LAVA	-5	// Lava contents.
#define	BSP_CONTENTS_SKY	-7	// Sky contents.
#define	BSP_CONTENTS_0		-9
#define	BSP_CONTENTS_DOWN	-14	// Down current.
#define	BSP_CONTENTS_ORIGIN	-15	// Origin brush, removed during CSG time.

enum
{
	LUMP_ENTITIES,
	LUMP_PLANES,
	LUMP_TEXTURES,
	LUMP_VERTEXES,
	LUMP_VISIBILITY,
	LUMP_NODES,
	LUMP_TEXINFO,
	LUMP_FACES,
	LUMP_LIGHTING,
	LUMP_CLIPNODES,
	LUMP_LEAFS,
	LUMP_MARKSURFACES,
	LUMP_EDGES,
	LUMP_SURFEDGES,
	LUMP_MODELS,

	HEADER_LUMPS
};

typedef enum
{
	BSP_AMBIENT_WATER,
	BSP_AMBIENT_SKY,
	BSP_AMBIENT_SLIME,
	BSP_AMBIENT_LAVA,

	BSP_AMBIENT_END
} BSPAmbient_t;

typedef struct
{
	int	iFileOffset,iFileLength;
} BSPLump_t;

typedef struct
{
	int			iIdent,iVersion;
	BSPLump_t	bLumps[HEADER_LUMPS];
} BSPHeader_t;

typedef struct
{
	float	fMins[3],fMaxs[3],
			fOrigin[3];

	int		iHeadNode[BSP_MAX_HULLS],
			iVisLeafs;

	unsigned int
		iFirstFace,
		iNumFaces;
} BSPModel_t;

typedef struct
{
	float	fPoint[3];
} BSPVertex_t;

typedef struct
{
	float fNormal[3];
	float fDist;

	int	iType;
} BSPPlane_t;

typedef struct
{
	unsigned int		iPlaneNum;

	int					iChildren[2];

	float				fMins[3],fMaxs[3];

	unsigned int		usFirstFace,
						usNumFaces;
} BSPNode_t;

typedef struct
{
	int		iPlaneNum,
			iChildren[2];
} BSPClipNode_t;

typedef struct
{
	float			v[2][4];
	unsigned int	iMipTex;
	int				iFlags;
} BSPTextureInfo_t;

typedef struct
{
	unsigned	int	v[2];
} BSPEdge_t;

typedef struct
{
	int		iPlaneNum,
			iSide,
			iFirstEdge,
			iNumEdges,
			iTexInfo;

	unsigned char bStyles[4];

	int		iLightOffset;
} BSPFace_t;

typedef struct
{
	int					iContents,
						iVisibilityOffset;

	float				fMins[3],fMaxs[3];

	unsigned	int		uiFirstMarkSurface,
						uiNumMarkSurfaces;

	PLuchar	bAmbientLevel[BSP_AMBIENT_END];
} BSPLeaf_t;
