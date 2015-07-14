/*	Copyright (C) 2011-2015 OldTimes Software

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

#include "EngineBase.h"

#include "EngineVideoMaterial.h"
#include "EngineEditor.h"
#include "EngineVideo.h"

Material_t *mActiveMaterial = NULL; // This is the material we're currently editing.
Material_t *mFloorMaterial;			// Material used for the floor plane.

model_t *mCubeModel;
model_t *mPlaneModel;
model_t *mSphereModel;

DynamicLight_t *dlMainLight;

ClientEntity_t *mPreviewEntity = NULL;

bool bMaterialEditorInitialized = false;

void MaterialEditor_Initialize(void)
{
	if (bMaterialEditorInitialized)
		return;

	CL_Disconnect();
	Host_ShutdownServer(false);

	cls.state = CLIENT_STATE_EDITOR;

	key_dest = KEY_EDITOR_MATERIAL;

	mPreviewEntity = CL_NewTempEntity();
	if (!mPreviewEntity)
	{
		Con_Warning("Failed to allocation client entity!\n");
		return;
	}

	mFloorMaterial = Material_Load("engine/grid");
	if (!mFloorMaterial)
	{
		Con_Warning("Failed to load grid material!\n");
		return;
	}

	dlMainLight = Client_AllocDlight(0);
	dlMainLight->bLightmap = false;
	dlMainLight->decay = 0;
	dlMainLight->die = 0;
	dlMainLight->minlight = 32.0f;
	dlMainLight->radius = 200.0f;
	Math_VectorSet(-40.0f, dlMainLight->origin);
	Math_VectorSet(255.0f, dlMainLight->color);

	mCubeModel = Mod_ForName("models/placeholders/cube.md2");
	mSphereModel = Mod_ForName("models/placeholders/sphere.md2");
	if (!mSphereModel || !mCubeModel)
	{
		Con_Warning("Failed to load preview mesh!\n");
		return;
	}

	mPreviewEntity->alpha = 255;
	mPreviewEntity->origin[0] = 50.0f;
	mPreviewEntity->origin[1] = 0;
	mPreviewEntity->origin[2] = 0;
	mPreviewEntity->model = mSphereModel;

	bMaterialEditorInitialized = true;
}

void MaterialEditor_Input(int iKey)
{
	switch (iKey)
	{
	case K_DOWNARROW:
		break;
	}
}

typedef enum
{
	EDITOR_MESH_SPHERE,
	EDITOR_MESH_CUBE,
	EDITOR_MESH_PLANE
} EditorPreviewTypes_t;

void MaterialEditor_SetPreviewMesh(EditorPreviewTypes_t previewMesh)
{
	switch (previewMesh)
	{
	case EDITOR_MESH_CUBE:
		mPreviewEntity->model = mCubeModel;
		break;
	case EDITOR_MESH_PLANE:
		Con_Warning("Not implemented!\n");
		break;
	case EDITOR_MESH_SPHERE:
		mPreviewEntity->model = mSphereModel;
		break;
	default:
		Con_Warning("Invalid preview type! (%i)", previewMesh);
	}
}

#define	MATERIALEDITOR_FLOOR_SCALE	10.0f

void MaterialEditor_Draw(void)
{
#if 0
	MathVector3f_t mvPosition = { 0, 0, -5 };
	Draw_Grid(mvPosition, 3);

	{
		VideoObjectVertex_t	voGrid[4] = { 0 };

		Video_ResetCapabilities(false);

		glPushMatrix();
		glTranslatef(10.0f, 0, 0);
		glRotatef(-90.0f, 0, 1, 0);
		glRotatef(-90.0f, 0, 0, 1);
		Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);
		Video_SetColour(1.0f, 1.0f, 1.0f, 1.0f);
		Video_ObjectVertex(&voGrid[0], -5, 5, 0);
		Video_ObjectTexture(&voGrid[0], VIDEO_TEXTURE_DIFFUSE, 0, 0);
		Video_ObjectVertex(&voGrid[1], 5, 5, 0);
		Video_ObjectTexture(&voGrid[1], VIDEO_TEXTURE_DIFFUSE, 1.0f, 0);
		Video_ObjectVertex(&voGrid[2], 5, -5, 0);
		Video_ObjectTexture(&voGrid[2], VIDEO_TEXTURE_DIFFUSE, 1.0f, 1.0f);
		Video_ObjectVertex(&voGrid[3], -5, -5, 0);
		Video_ObjectTexture(&voGrid[3], VIDEO_TEXTURE_DIFFUSE, 0, 1.0f);
		Video_DrawFill(voGrid, mActiveMaterial, 0);
		glPopMatrix();

		Video_ResetCapabilities(true);
	}
#endif

	if (cvVideoDrawModels.bValue)
		Alias_Draw(mPreviewEntity);

	GL_SetCanvas(CANVAS_DEFAULT);
	Draw_String(10, 10, va("Camera: origin(%i %i %i), angles(%i %i %i)",
		(int)r_refdef.vieworg[pX],
		(int)r_refdef.vieworg[pY],
		(int)r_refdef.vieworg[pZ],
		(int)r_refdef.viewangles[pX],
		(int)r_refdef.viewangles[pY],
		(int)r_refdef.viewangles[pZ]));
	Draw_String(10, 20, va("Model: %s",	mPreviewEntity->model->name));
	if (mActiveMaterial)
		Draw_String(10, 30, va("Material: %s", mActiveMaterial->cPath));
}

void MaterialEditor_Frame(void)
{
	mPreviewEntity->angles[1] += 0.5f;
}

void MaterialEditor_Display(Material_t *mDisplayMaterial)
{
	if (!bMaterialEditorInitialized)
	{
		Con_Warning("The editor hasn't been initialized!\n");
		return;
	}

	mActiveMaterial = mDisplayMaterial;
	if (!mActiveMaterial)
	{
		Con_Warning("Failed to display material!");
		return;
	}

	mPreviewEntity->model->mAssignedMaterials = mActiveMaterial;
}

void MaterialEditor_Shutdown(void)
{}