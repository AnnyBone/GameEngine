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

plAnimatedModel_t *plCreateAnimatedModel()
{
	plSetErrorFunction("plCreateModel");

	plAnimatedModel_t *model = new plAnimatedModel_t;
	if (!model)
		return nullptr;

	memset(model, 0, sizeof(plAnimatedModel_t));

	return model;
}

void plDeleteAnimatedModel(plAnimatedModel_t *model)
{
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
	UNREAL 3D Model Format

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

	plAnimatedModel_t *model = new plAnimatedModel_t;

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

			_plUnloadU3DFiles();
			return nullptr;
		}

		for (unsigned int j = 0; j < model->num_triangles; j++)
		{

		}
	}

	// Calculate normals.

	_plUnloadU3DFiles();

	return model;
}