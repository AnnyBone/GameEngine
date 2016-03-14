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
	UNREAL 3D Model Format

	The following is based on information from the following page...
	http://paulbourke.net/dataformats/unreal/
*/

#include "../shared/format_u3d.h"

FILE *pl_u3d_dataf = nullptr;
FILE *pl_u3d_animf = nullptr;

void _plUnloadU3DFiles()
{
	if (pl_u3d_animf)
		std::fclose(pl_u3d_animf);
	if (pl_u3d_dataf)
		std::fclose(pl_u3d_dataf);
}

plModel_t *plLoadU3DModel(const char *path)
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

	// TODO: replace
	plModel_t *model = new plModel_t;

	// Store the information we've gathered.
	model->num_frames		= animheader.frames;
	model->num_triangles	= dataheader.numpolys;
	model->num_vertices		= dataheader.numverts;

	// If it has more than one frame, we're gonna want to interp between
	// it all later.
	if (model->num_frames > 1)
		model->type = PL_MODEL_TYPE_ANIMATED;

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