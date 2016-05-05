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

/*
	UNREAL 3D Animated Model Format

	The following is based on information from the following page...
	http://paulbourke.net/dataformats/unreal/
*/

#define	U3D_FILE_EXTENSION "3d"

typedef struct U3DAnimationHeader_s
{
	uint16_t	frames;	// Number of frames.
	uint16_t	size;	// Size of each frame.
} U3DAnimationHeader_t;

typedef struct U3DDataHeader_s
{
	uint16_t	numpolys;	// Number of polygons.
	uint16_t	numverts;	// Number of vertices.
	uint16_t	rotation;	// Mesh rotation?
	uint16_t	frame;		// Initial frame.

	uint32_t	norm_x;
	uint32_t	norm_y;
	uint32_t	norm_z;

	uint32_t	fixscale;
	uint32_t	unused[3];
} U3DDataHeader_t;

#define	U3D_FLAG_UNLIT			16
#define	U3D_FLAG_FLAT			32
#define	U3D_FLAG_ENVIRONMENT	64
#define	U3D_FLAG_NEAREST		128

enum U3DType
{
	U3D_TYPE_NORMAL,
	U3D_TYPE_NORMALTWOSIDED,
	U3D_TYPE_TRANSLUCENT,
	U3D_TYPE_MASKED,
	U3D_TYPE_MODULATE,
	U3D_TYPE_ATTACHMENT
};

typedef struct U3DVertex_s
{
	// This is a bit funky...
	int32_t x : 11;
	int32_t y : 11;
	int32_t z : 10;
} U3DVertex_t;

typedef struct U3DTriangle_s
{
	uint16_t vertex[3];	// Vertex indices

	uint8_t	type;		// Triangle type
	uint8_t colour;		// Triangle colour
	uint8_t ST[3][2];	// Texture coords
	uint8_t texturenum;	// Texture offset
	uint8_t flags;		// Triangle flags
} U3DTriangle_t;

FILE *pl_u3d_dataf = nullptr;
FILE *pl_u3d_animf = nullptr;

void _plUnloadU3DFiles()
{
	if (pl_u3d_animf)
		std::fclose(pl_u3d_animf);
	if (pl_u3d_dataf)
		std::fclose(pl_u3d_dataf);
}

plAnimatedModel_t *plLoadU3DModel(const char *path)
{
	plSetErrorFunction("plLoadU3DModel");

	pl_u3d_dataf = std::fopen(path, "rb");
	if (!pl_u3d_dataf)
	{
		plSetError("Failed to load data file! (%s)\n", path);

		_plUnloadU3DFiles();
		return nullptr;
	}

	// Try to figure out the data string we used
	// then erase it.
	std::string newpath(path);
	size_t strpos = newpath.find("_d.3d");
	if (strpos == std::string::npos)
		// Some legacy models use _Data...
		strpos = newpath.find("_Data.3d");
		
	if (strpos != std::string::npos)
		newpath.erase(strpos);
	else
	{
		plSetError("Invalid file name! (%s)\n", newpath.c_str());

		_plUnloadU3DFiles();
		return nullptr;
	}
	newpath.append("_a.3d");

	// Attempt to load the animation file.
	pl_u3d_animf = std::fopen(newpath.c_str(), "rb");
	if (!pl_u3d_animf)
	{
		// Some legacy models use _Anim...
		newpath.erase(newpath.length() - 5);
		newpath.append("_Anim.3d");

		pl_u3d_animf = std::fopen(newpath.c_str(), "r");
		if (!pl_u3d_animf)
		{
			plSetError("Failed to load U3D animation data! (%s)\n", newpath.c_str());

			_plUnloadU3DFiles();
			return nullptr;
		}
	}

	// Attempt to read the animation header.
	U3DAnimationHeader_t animheader;
	if (std::fread(&animheader, sizeof(U3DAnimationHeader_t), 1, pl_u3d_animf) != 1)
	{
		plSetError("Failed to read animation file!\n");

		_plUnloadU3DFiles();
		return nullptr;
	}

	// Attempt to read the data header.
	U3DDataHeader_t dataheader;
	if (std::fread(&dataheader, sizeof(U3DDataHeader_t), 1, pl_u3d_dataf) != 1)
	{
		plSetError("Failed to read data file!\n");

		_plUnloadU3DFiles();
		return nullptr;
	}

	plAnimatedModel_t *model = plCreateAnimatedModel();
	if (!model)
	{
		plSetError("Failed to allocate animated model!\n");
		
		_plUnloadU3DFiles();
		return nullptr;
	}

	// Store the information we've gathered.
	model->num_frames		= animheader.frames;
	model->num_triangles	= dataheader.numpolys;
	model->num_vertices		= dataheader.numverts;

	// Allocate the triangle/vertex arrays.
	model->frames = new plModelFrame_t[model->num_frames];
	for (unsigned int i = 0; i < model->num_frames; i++)
	{
		model->frames[i].vertices = new plVertex_t[model->num_vertices];
		model->frames[i].triangles = new plTriangle_t[model->num_triangles];
	}

	// Skip unused header data.
	std::fseek(pl_u3d_dataf, 12, SEEK_CUR);

	// Go through each triangle.
	std::vector<U3DTriangle_t> utriangles;
	for (unsigned int i = 0; i < model->num_triangles; i++)
	{
		if (std::fread(&utriangles[i], sizeof(U3DTriangle_t), 1, pl_u3d_dataf) != 1)
		{
			plSetError("Failed to process triangles! (%i)\n", i);

			plDeleteAnimatedModel(model);

			_plUnloadU3DFiles();
			return nullptr;
		}

		// Copy the indices over.
		model->frames[0].triangles[i].indices[0] = utriangles[i].vertex[0];
		model->frames[0].triangles[i].indices[1] = utriangles[i].vertex[1];
		model->frames[0].triangles[i].indices[2] = utriangles[i].vertex[2];
	}

	// Go through each vertex.
	std::vector<U3DVertex_t> uvertices;
	for (unsigned int i = 0; i < model->num_frames; i++)
	{
		if (std::fread(&uvertices[i], sizeof(U3DVertex_t), 1, pl_u3d_animf) != 1)
		{
			plSetError("Failed to process vertex! (%i)\n", i);

			plDeleteAnimatedModel(model);

			_plUnloadU3DFiles();
			return nullptr;
		}

		for (unsigned int j = 0; j < model->num_triangles; j++)
		{

		}
	}

	// Calculate normals.
	plGenerateAnimatedModelNormals(model);

	_plUnloadU3DFiles();

	return model;
}