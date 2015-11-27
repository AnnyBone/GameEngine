/*	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef FORMAT_U3D_H
#define	FORMAT_U3D_H

/*
	UNREAL 3D Model Format

	The following is based on information from the following page...
	http://paulbourke.net/dataformats/unreal/
*/

#define	FORMAT_U3D_EXTENSION	".3d"

typedef struct
{
	uint16_t	frames;	// Number of frames.
	uint16_t	size;	// Size of each frame.
} U3DAnimationHeader;

typedef struct
{
	uint16_t	numpolys;	// Number of polygons.
	uint16_t	numverts;	// Number of vertices.
	uint16_t	rotation;	// Mesh rotation?
	uint16_t	frame;		// Initial frame??

	uint32_t	norm_x;
	uint32_t	norm_y;
	uint32_t	norm_z;

	uint32_t	fixscale;
	uint32_t	unused[3];
} U3DDataHeader;

typedef struct
{
	int32_t x, y, z;
} U3DVertex;

enum U3DFlags
{
	U3D_FLAG_TEXTURED,
	U3D_FLAG_FLAT,
	U3D_FLAG_TRANSPARENT,
	U3D_FLAG_MASKED
};

enum U3DType
{
	U3D_TYPE_NORMAL,
	U3D_TYPE_NORMALTWOSIDED,
	U3D_TYPE_TRANSLUCENT,
	U3D_TYPE_MASKED,
	U3D_TYPE_MODULATE,
	U3D_TYPE_ATTACHMENT
};

typedef struct
{
	uint16_t	vertex[3];	// Vertex indices
	
	uint8_t	type;		// Triangle type
	uint8_t colour;		// Triangle colour
	uint8_t ST[3][2];	// Texture coords
	uint8_t texturenum;	// Texture offset
	uint8_t flags;		// Triangle flags
} U3DTriangle;

#endif // !FORMAT_U3D_H