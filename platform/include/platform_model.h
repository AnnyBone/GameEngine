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

enum
{
	PL_MODELTYPE_START,

	PL_MODELTYPE_STATIC,
	PL_MODELTYPE_ANIMATED,
	PL_MODELTYPE_SKELETAL,

	PL_MODELTYPE_END
};

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

	vlPrimitive_t primitive;

	plModelFrame_t	frame;
} plStaticModel_t;

/*	Per-vertex animated mesh.
*/
typedef struct plAnimatedModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;
	unsigned int num_frames;

	vlPrimitive_t primitive;

	plModelFrame_t *frames;
} plAnimatedModel_t;

/*	Mesh with bone structure.
*/
typedef struct plSkeletalModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;

	vlPrimitive_t primitive;

	// Unfinished...
} plSkeletalModel_t;

#include "platform_model_u3d.h"
#include "platform_model_obj.h"

plEXTERN_C_START

// Static
plStaticModel_t *plCreateStaticModel(void);
plStaticModel_t *plLoadStaticModel(const char *path);
void plDeleteStaticModel(plStaticModel_t *model);

// Animated
plAnimatedModel_t *plCreateAnimatedModel(void);
plAnimatedModel_t *plLoadAnimatedModel(const char *path);
void plDeleteAnimatedModel(plAnimatedModel_t *model);

plAnimatedModel_t *plLoadU3DModel(const char *path);

// Utility
void plGenerateStaticModelNormals(plStaticModel_t *model);
void plGenerateAnimatedModelNormals(plAnimatedModel_t *model);
void plGenerateSkeletalModelNormals(plSkeletalModel_t *model);

plEXTERN_C_END