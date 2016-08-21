/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#include "platform.h"
#include "platform_math.h"
#include "platform_video_layer.h"

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

	plVector3f_t mins, maxs; // Bounds
} plModelFrame_t;

/*	Static animated mesh.
*/
typedef struct plStaticModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;

	VLPrimitive primitive;

	plModelFrame_t	frame;
} plStaticModel_t;

/*	Per-vertex animated mesh.
*/
typedef struct plAnimatedModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;
	unsigned int num_frames;

	VLPrimitive primitive;

	plModelFrame_t *frames;
} plAnimatedModel_t;

/*	Mesh with bone structure.
*/
typedef struct plSkeletalModel_s
{
	unsigned int num_triangles;
	unsigned int num_vertices;

	VLPrimitive primitive;

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