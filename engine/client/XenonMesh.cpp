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

typedef struct XMesh
{
	char name[PL_MAX_PATH];

	vlDraw_t **draw_calls;
} XMesh;

class XMeshManager : public XManager
{
public:
	XMeshManager();
	~XMeshManager();

	void Frame() {}

	XMesh *LoadMesh(std::string path);	// Loads a new mesh.
	XMesh *GetMesh(std::string path);	// Gets an existing mesh.

	void ClearMeshes() {}	// Clear all meshes.

private:
	XMesh *LoadStaticModel(PLStaticModel *model);
	XMesh *LoadAnimatedModel(plAnimatedModel_t *model);

	std::unordered_map<std::string, XMesh*> meshes;
};

XMeshManager::XMeshManager()
{
	Con_Printf("Initializing Mesh Manager...\n");
}

XMeshManager::~XMeshManager()
{
	ClearMeshes();
}

typedef struct XMeshType
{
	const char *extension;

	unsigned int type;
} XMeshType;

XMeshType mesh_formatlist[] =
{
	{ "u3d", PL_MODELTYPE_ANIMATED },
	{ "obj", PL_MODELTYPE_STATIC },
};

XMesh *XMeshManager::LoadMesh(std::string path)
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
				PLStaticModel *static_model = plLoadStaticModel(path.c_str());
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