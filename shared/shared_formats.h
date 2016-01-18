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

#ifndef	SHARED_FORMATS_H
#define	SHARED_FORMATS_H

typedef enum
{
#if 1
	MODEL_TYPE_MD2,		// MD2 support.
	MODEL_TYPE_IQM,		// IQM support.
	MODEL_TYPE_OBJ,		// OBJ support.
#endif
	/*	TODO:
			Eventually I want to convert meshes
			during load time to somewhat of an abstrac
			type, so it's no longer format-specific.
	*/
	MODEL_TYPE_STATIC,		// Static mesh (MD2, U3D, OBJ)
	MODEL_TYPE_SKELETAL,	// Skeletal mesh (IQM)
	MODEL_TYPE_VERTEX,		// Per-vertex animated mesh (MD2, U3D)
	MODEL_TYPE_LEVEL,		// Level / World model.

	MODEL_NONE
} ModelType_t;

#include "format_u3d.h"	// UNREAL 3D
#include "format_obj.h"	// OBJ
#include "format_iqm.h"	// INTER-QUAKE MODEL

/*
	KMDL Format
	Extension of the already existing MD2 format.
*/

#define MD2E_HEADER		(('E'<<24)+('2'<<16)+('D'<<8)+'M')
#define	MD2E_VERSION	9
#define	MD2E_EXTENSION	".model"

/*
	MD2 Format
*/

#define MD2_HEADER		(('2'<<24)+('P'<<16)+('D'<<8)+'I')
#define	MD2_EXTENSION	".md2"

#define	MD2_VERSION	8

#define	MD2_MAX_FRAMES		1024
#define MD2_MAX_SKINS		32
#define MD2_MAX_TRIANGLES	4096
#define	MD2_MAX_VERTICES	8192

typedef struct
{
	short	index_xyz[3];
	short	index_st[3];
} MD2Triangle_t;

typedef struct
{
	pUCHAR	v[3];				// scaled byte to fit in frame mins/maxs
	pUCHAR	lightnormalindex;
} MD2TriangleVertex_t;

typedef struct
{
	float scale[3];					// multiply byte verts by this
	float translate[3];				// then add this
	char name[16];					// frame name from grabbing
	MD2TriangleVertex_t	verts[1];	// variable sized
} MD2Frame_t;

typedef struct
{
	short	S,T;
} MD2TextureCoordinate_t;

typedef struct
{
	int			    ident;
	int			    version;
	unsigned    int	skinwidth;
	unsigned    int	skinheight;
	int			    framesize;		// Byte size of each frame.
	int			    num_skins;
	int			    num_xyz;
	int			    num_st;			// Greater than num_xyz for seams.
	int			    numtris;
	int			    num_glcmds;		// Dwords in strip/fan command list.
	int			    num_frames;
	int			    ofs_skins;		// Each skin is a MAX_SKINNAME string.
	int			    ofs_st;			// Byte offset from start for stverts.
	int			    ofs_tris;		// Offset for dtriangles.
	int			    ofs_frames;		// Offset for first frame.
	int			    ofs_glcmds;
	int			    ofs_end;		// End of file.
} MD2_t;

/*
	BSP Format
*/

#define	BSP_HEADER		(('4'<<24)+('L'<<16)+('V'<<8)+'L')	// For easy identification.
#define	BSP_HEADER_SIZE	8									// "BSP" followed by version number.

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
	int					iPlaneNum,
						iChildren[2];

	float				fMins[3],fMaxs[3];

	unsigned	int		usFirstFace,
						usNumFaces;
} BSPNode_t;

typedef struct
{
	int		iPlaneNum,
			iChildren[2];
} BSPClipNode_t;

typedef struct
{
	float	v[2][4];
	int		iMipTex,
			iFlags;
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

	pUCHAR				bAmbientLevel[BSP_AMBIENT_END];
} BSPLeaf_t;

// Obsolete

typedef struct
{
	int			nummiptex;
	int			dataofs[4];		// [nummiptex]
} dmiptexlump_t;

typedef struct miptex_s
{
	char		name[16];
	unsigned	width,height;
} miptex_t;

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5

#endif // !SHARED_FORMATS_H
