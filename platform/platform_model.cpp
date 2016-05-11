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

#include "platform.h"

#include "platform_model.h"

/*
	PLATFORM MODEL LOADER
*/

std::vector<float> plGenerateNormal(plVector3f_t a, plVector3f_t b, plVector3f_t c)
{
	plVector3f_t x, y;
	plVectorSubtract3fv(c, b, x);
	plVectorSubtract3fv(a, b, y);

	plVector3f_t normal;
	plVectorClear(normal);
	Math_CrossProduct(x, y, normal);
	plVectorNormalize(normal);

	std::vector<float> vnorm = { normal[0], normal[1], normal[2] };
	return vnorm;
}

void plGenerateStaticModelNormals(plStaticModel_t *model)
{
	plModelFrame_t *frame = &model->frame;
#if 0 // per face...
	for (int i = 0; i < model->num_triangles; i++)
	{
		std::vector<float> normal = plGenerateNormal(
			frame->vertices[frame->triangles[i].indices[0]].position,
			frame->vertices[frame->triangles[i].indices[1]].position,
			frame->vertices[frame->triangles[i].indices[2]].position);
		
		frame->triangles[i].normal[PL_X] = normal[PL_X];
		frame->triangles[i].normal[PL_Y] = normal[PL_Y];
		frame->triangles[i].normal[PL_Z] = normal[PL_Z];
	}
#else // per vertex...
	for (plVertex_t *vertex = &frame->vertices[0]; vertex; ++vertex)
	{
		for (plTriangle_t *triangle = &frame->triangles[0]; triangle; ++triangle)
		{

		}
	}
#endif
}

void plGenerateAnimatedModelNormals(plAnimatedModel_t *model)
{
	if (!model)
		return;

	for (plModelFrame_t *frame = &model->frames[0]; frame; ++frame)
	{
		for (plVertex_t *vertex = &frame->vertices[0]; vertex; ++vertex)
		{
			for (plTriangle_t *triangle = &frame->triangles[0]; triangle; ++triangle)
			{

			}
		}
	}
}

void plGenerateSkeletalModelNormals(plSkeletalModel_t *model)
{
	if (!model)
		return;
}

/*
	Static Model
*/

plStaticModel_t *plCreateStaticModel(void)
{
	plSetErrorFunction("plCreateStaticModel");

	plStaticModel_t *model = new plStaticModel_t;
	if (!model)
		return nullptr;

	memset(model, 0, sizeof(plStaticModel_t));

	return model;
}

plStaticModel_t *plLoadStaticModel(const char *path)
{
	if (!path || path[0] == ' ')
		return nullptr;

	return nullptr;
}

void plDeleteStaticModel(plStaticModel_t *model)
{
	if (!model)
	{
		plSetError("Invalid model!\n");
		return;
	}

	if (model->frame.triangles)
		delete model->frame.triangles;
	if (model->frame.vertices)
		delete model->frame.vertices;

	delete model;
}

/*	
	Animated Model
*/

plAnimatedModel_t *plCreateAnimatedModel(void)
{
	plSetErrorFunction("plCreateAnimatedModel");

	plAnimatedModel_t *model = new plAnimatedModel_t;
	if (!model)
		return nullptr;
	
	memset(model, 0, sizeof(plAnimatedModel_t));

	return model;
}

plAnimatedModel_t *plLoadAnimatedModel(const char *path)
{
	if (!path || path[0] == ' ')
		return nullptr;

	return nullptr;
}

void plDeleteAnimatedModel(plAnimatedModel_t *model)
{
	plSetErrorFunction("plDeleteAnimatedModel");

	if (!model)
	{
		plSetError("Invalid model!\n");
		return;
	}

	for (unsigned int i = 0; i < model->num_frames; i++)
	{
		if (&model->frames[i])
		{
			if (model->frames[i].triangles)
				delete model->frames[i].triangles;
			if (model->frames[i].vertices)
				delete model->frames[i].vertices;
			delete &model->frames[i];
		}
	}

	delete model;
}

/*
	MD2 Model Format

	Model format introduced in id Software's Quake 2.
*/

#define MD2_HEADER			(('2'<<24)+('P'<<16)+('D'<<8)+'I')
#define	MD2_FILE_EXTENSION	".md2"
#define	MD2_VERSION			8

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
	pl_uchar v[3];				// scaled byte to fit in frame mins/maxs
	pl_uchar lightnormalindex;
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
	short	S, T;
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

plAnimatedModel_t *plLoadMD2Model(const char *path)
{
	if (!path || path[0] == ' ')
		return nullptr;

	return nullptr;
}

/*
	UNREAL PSKX Static Model Format

	Model format introduced in Unreal Engine 2.0, sadly rather
	hard to dig up much information about it.
*/

#define PSKX_EXTENSION "pskx"

