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

typedef struct plVertex_s
{
	plVector3f_t position, normal;

	plVector2f_t ST;

	plColour_t colour;
} plVertex_t;

typedef struct plTriangle_s
{
	plVector3f_t normal;

	unsigned int indices[3];
} plTriangle_t;

typedef struct plModelFrame_s
{
	plTriangle_t	*triangles;
	plVertex_t		*vertices;
} plModelFrame_t;

/*	Static animated mesh.
*/
typedef struct plStaticModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;

	plModelFrame_t	frame;
} plStaticModel_t;

/*	Per-vertex animated mesh.
*/
typedef struct plAnimatedModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;
	unsigned int num_frames;

	plModelFrame_t *frames;
} plAnimatedModel_t;

/*	Mesh with bone structure.
*/
typedef struct plSkeletalModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;

	// Unfinished...
} plSkeletalModel_t;

plEXTERN_C_START

// Animated
plAnimatedModel_t *plLoadU3DModel(const char *path);

plEXTERN_C_END