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

#include "engine_base.h"

using namespace std;

/*	Loads a U3D model file.
	TODO: 
		Reverse faces.
		Get and set UV coords correctly.
		Crash occuring when failing to get name?
		Support for multiple textures, somehow...
		Animated frames are mangled.
*/
bool ModelU3D_Load(model_t *model, void *buf)
{
	// We don't need your kind here!
	free(buf);

	FILE *dataf;
	COM_FOpenFile(model->name, &dataf);
	if (!dataf)
	{
		Con_Warning("Failed to load data file!\n");
		return false;
	}

	// Try to figure out the data string we used
	// then erase it.
	string newpath(model->name);
	size_t strpos = newpath.find("_d.3d");
	if (strpos != string::npos)
		newpath.erase(strpos);
	else
	{
		// Some legacy models use _Data...
		strpos = newpath.find("_Data.3d");
		if (strpos != string::npos)
			newpath.erase(strpos);
		else
		{
			fclose(dataf);

			Con_Warning("Invalid file name! (%s)\n", newpath.c_str());
			return false;
		}
	}
	newpath.append("_a.3d");

	// Attempt to load the animation file.
	FILE *animf;
	COM_FOpenFile(newpath.c_str(), &animf);
	if (!animf)
	{
		// Some legacy models use _Anim...
		newpath.erase(newpath.length() - 2);
		newpath.append("_Anim.3d");

		COM_FOpenFile(newpath.c_str(), &animf);
		if (!animf)
		{
			fclose(dataf);

			// Welp, I give up *drops everything and walks out*
			Con_Warning("Failed to load animation file!\n");
			return false;
		}
	}

	// Attempt to read the animation header.
	U3DAnimationHeader_t animheader;
	if (fread(&animheader, sizeof(U3DAnimationHeader_t), 1, animf) != 1)
	{
		fclose(animf);
		fclose(dataf);

		Con_Warning("Failed to read animation file!\n");
		return false;
	}

	// Attempt to read the data header.
	U3DDataHeader_t	dataheader;
	if (fread(&dataheader, sizeof(U3DDataHeader_t), 1, dataf) != 1)
	{
		fclose(animf);
		fclose(dataf);

		Con_Warning("Failed to read data file!\n");
		return false;
	}

	// Store the information we've gathered.
	model->numtriangles		= dataheader.numpolys;
	model->numvertexes		= dataheader.numverts;
	model->numframes		= animheader.frames;

	// Allocate an object for each frame.
	model->objects = (VideoObject_t*)calloc(model->numframes, sizeof(VideoObject_t));

	// If it has more than one frame, we're gonna want to interp between
	// it all later.
	if (model->numframes > 1)
		model->type = MODEL_TYPE_VERTEX;

	fseek(dataf, 12, SEEK_CUR);

	// Process all the triangles...
	U3DTriangle_t *utriangles = (U3DTriangle_t*)calloc(model->numtriangles, sizeof(U3DTriangle_t));
	for (unsigned int i = 0; i < model->numtriangles; i++)
	{
		size_t retsize = fread(&utriangles[i], sizeof(U3DTriangle_t), 1, dataf);
		if (retsize != 1)
		{
			free(utriangles);

			fclose(animf);
			fclose(dataf);

			Con_Warning("Failed to process triangles! (%i)\n", retsize);
			return false;
		}
	}

	// Process all the vertices...
	U3DVertex_t *uvertices = (U3DVertex_t*)calloc((model->numvertexes * model->numframes), sizeof(U3DVertex_t));
	for (int i = 0; i < model->numframes; i++)
	{
		// Allocate vertices.
		model->objects[i].vertices		= (VideoVertex_t*)calloc(model->numvertexes, sizeof(VideoVertex_t));
		model->objects[i].numverts		= model->numvertexes;
		model->objects[i].numtriangles	= model->numtriangles;
		model->objects[i].primitive		= VIDEO_PRIMITIVE_TRIANGLES;
		model->objects[i].indices		= (uint8_t*)calloc(model->numtriangles * 3, sizeof(uint8_t));

		size_t retsize = fread(&uvertices[i], sizeof(U3DVertex_t), model->numvertexes, animf);
		if (retsize != (size_t)model->numvertexes)
		{
			free(uvertices);
			fclose(animf);
			free(utriangles);
			fclose(dataf);

			Con_Warning("Failed to process vertices! (%i)\n", retsize);
			return false;
		}

		// Copy the indices over.
		for (unsigned int j = 0, k = 0; j < model->numtriangles; j++)
		{
			model->objects[i].indices[k] = utriangles[j].vertex[0];	k++;
			model->objects[i].indices[k] = utriangles[j].vertex[1];	k++;
			model->objects[i].indices[k] = utriangles[j].vertex[2];	k++;

			model->objects[i].vertices[utriangles[j].vertex[0]].mvST[0][0] = utriangles[j].ST[0][0];
			model->objects[i].vertices[utriangles[j].vertex[0]].mvST[0][1] = utriangles[j].ST[0][1];
			model->objects[i].vertices[utriangles[j].vertex[1]].mvST[0][0] = utriangles[j].ST[1][0];
			model->objects[i].vertices[utriangles[j].vertex[1]].mvST[0][1] = utriangles[j].ST[1][1];
			model->objects[i].vertices[utriangles[j].vertex[2]].mvST[0][0] = utriangles[j].ST[2][0];
			model->objects[i].vertices[utriangles[j].vertex[2]].mvST[0][1] = utriangles[j].ST[2][1];
		}

		// Copy each of the vertices over.
		for (int j = 0; j < model->numvertexes; j++)
		{
			model->objects[i].vertices[j].mvPosition[0] = (float)uvertices[j].x;
			model->objects[i].vertices[j].mvPosition[1] = (float)uvertices[j].y;
			model->objects[i].vertices[j].mvPosition[2] = (float)uvertices[j].z;

			Math_Vector4Copy(g_colourwhite, model->objects[i].vertices[j].mvColour);
		}

		fseek(animf, animheader.size - model->numvertexes * sizeof(U3DVertex_t), SEEK_CUR);
	}
	free(uvertices);
	fclose(animf);
	free(utriangles);
	fclose(dataf);

#if 0
		for (unsigned int j = 0, verts = 0; j < model->numtriangles; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				model->objects[i].vertices[utriangles[j].vertex[k]].mvPosition[0] = (float)uvertices[utriangles[j].vertex[k]].x;
				model->objects[i].vertices[utriangles[j].vertex[k]].mvPosition[1] = (float)uvertices[utriangles[j].vertex[k]].y;
				model->objects[i].vertices[utriangles[j].vertex[k]].mvPosition[2] = (float)uvertices[utriangles[j].vertex[k]].z;
				
				verts++;
			}
		}
#endif

	Model_LoadRelativeMaterial(model);

	// Everything checks out!
	return true;
}