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
		for (unsigned int i = 0; i < plArrayElements(mesh_formatlist); i++)
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
				{
					plAnimatedModel_t *animated_model = plLoadAnimatedModel(path.c_str());
					if (!animated_model)
					{
						Con_Warning("Failed to load animated model!\n");
						return nullptr;
					}
				}
				break;
				case PL_MODELTYPE_SKELETAL:
				{

				}
				break;
				default:
					Con_Warning("Unknown or unsupported model type!\n");
					return nullptr;
				}
			}
		}

		return nullptr;
	}
}