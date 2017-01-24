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

#ifndef FORMAT_IQM_H
#define	FORMAT_IQM_H

/*
	Inter-Quake Model Format

	http://sauerbraten.org/iqm/
*/

#define IQM_HEADER		"INTERQUAKEMODEL"
#define	IQM_EXTENSION	".iqm"

#define	IQM_VERSION	2

#define IQM_LOOP	1<<0

enum
{
	IQM_POSITION,
	IQM_TEXCOORD,
	IQM_NORMAL,
	IQM_TANGENT,
	IQM_BLENDINDEXES,
	IQM_BLENDWEIGHTS,
	IQM_COLOR,
	IQM_CUSTOM = 0x10
};

enum
{
	IQM_BYTE,
	IQM_UBYTE,
	IQM_SHORT,
	IQM_USHORT,
	IQM_INT,
	IQM_UINT,
	IQM_HALF,
	IQM_FLOAT,
	IQM_DOUBLE
};

typedef struct
{
	char			cMagic[16];
	unsigned int	uiVersion;
	unsigned int	uiFileSize;
	unsigned int	uiFlags;
	unsigned int	uiNumText, uiOfsText;
	unsigned int	uiNumMeshes, uiOfsMeshes;
	unsigned int	uiNumVertexArrays, uiNumVertexes, uiOfsVertexArrays;
	unsigned int	uiNumTriangles, uiOfsTriangles, uiOfsAdjacency;
	unsigned int	uiNumJoints, uiOfsJoints;
	unsigned int	uiNumPoses, uiOfsPoses;
	unsigned int	uiNumAnims, uiOfsAnims;
	unsigned int	uiNumFrames, uiNumFrameChannels, uiOfsFrames, uiOfsBounds;
	unsigned int	uiNumComment, uiOfsComment;
	unsigned int	uiNumExtensions, uiOfsExtensions;
} IQMHeader_t;

typedef struct
{
	unsigned int	uiName;
	unsigned int	uiMaterial;
	unsigned int	uiFirstVertex, uiNumVertexes;
	unsigned int	uiFirstTriangle, uiNumTriangles;
} IQMMesh_t;

typedef struct
{
	unsigned int	uiVertex[3];
} IQMTriangle_t;

typedef struct
{
	unsigned int	uiName;
	int				iParent;
	float			fTranslate[3], fRotate[4], fScale[3];
} IQMJoint_t;

typedef struct
{
	int				iParent;
	unsigned int	uiMask;
	float			fChannelOffset[10];
	float			fChannelScale[10];
} IQMPose_t;

typedef struct
{
	unsigned int	uiName;
	unsigned int	uiFirstFrame, uiNumFrames;
	float			fFrameRate;
	unsigned int	uiFlags;
} IQMAnim_t;

typedef struct
{
	unsigned int	uiType;
	unsigned int	uiFlags;
	unsigned int	uiFormat;
	unsigned int	uiSize;
	unsigned int	uiOffset;
} IQMVertexArray_t;

typedef struct
{
	float	fBBMin[3], fBBMax[3];
	float	fXYRadius, fRadius;
} IQMBounds_t;

#endif // !FORMAT_IQM_H
