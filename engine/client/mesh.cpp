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

#include "platform_model.h"

namespace Core
{
	typedef struct Mesh_s
	{
		char name[PL_MAX_PATH];

		vlDraw_t **draw_calls;
	} Mesh_t;

	class MeshManager : public CoreManager
	{
	public:
		MeshManager();
		~MeshManager();

		void Frame() {}

		Mesh_t *LoadMesh(std::string path);	// Loads a new mesh.
		Mesh_t *GetMesh(std::string path);	// Gets an existing mesh.

		void ClearMeshes() {}	// Clear all meshes.

	private:
		Mesh_t *LoadStaticModel(plStaticModel_t *model);
		Mesh_t *LoadAnimatedModel(plAnimatedModel_t *model);

		std::unordered_map<std::string, Mesh_t*> meshes;
	};

	MeshManager::MeshManager()
	{
		Con_Printf("Initializing Mesh Manager...\n");
	}

	MeshManager::~MeshManager()
	{
		ClearMeshes();
	}

	typedef struct MeshType_s
	{
		const char *extension;

		unsigned int type;
	} MeshType_t;

	MeshType_t mesh_formatlist[]=
	{
		{ "u3d", PL_MODELTYPE_ANIMATED },
		{ "obj", PL_MODELTYPE_STATIC },
	};

	Mesh_t *MeshManager::LoadMesh(std::string path)
	{
		// Grab the file extension, so we can check for formats that don't have idents.
		char extension[64];
		strncpy(extension, COM_FileExtension(path.c_str()), sizeof(extension));
		if (extension[0] == ' ')
		{
			Con_Warning("Failed to grab file extension!\n");
			return nullptr;
		}

		std::string fullpath = (host_parms.basepath + path);
		for (unsigned int i = 0; i < pARRAYELEMENTS(mesh_formatlist); i++)
		{
			if (strcmp(mesh_formatlist[i].extension, extension))
			{
				switch (mesh_formatlist[i].type)
				{
				case PL_MODELTYPE_STATIC:
				{
					plStaticModel_t *static_model = plLoadStaticModel(path.c_str());
					if (!static_model)
					{
						Con_Warning("Failed to load static model!\n");
						return nullptr;
					}
				}
				break;
				case PL_MODELTYPE_ANIMATED:
				{}
				break;
				case PL_MODELTYPE_SKELETAL:
					break;
				default:
					Con_Warning("Unknown or unsupported model type!\n");
					return nullptr;
				}
			}
		}
	}
}