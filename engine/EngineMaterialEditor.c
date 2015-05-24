/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "quakedef.h"

#include "EngineVideoMaterial.h"
#include "engine_editor.h"
#include "video.h"
#include "KatAlias.h"

Material_t *mActiveMaterial = NULL; // This is the material we're currently editing.
model_t *mPreviewModel = NULL;		// Model we're previewing material on.
MD2_t *mPreviewMesh = NULL;			// Mesh we're previewing the material on.

bool bMaterialEditorInitialized = false;

void MaterialEditor_Initialize(void)
{
	if (bMaterialEditorInitialized)
		return;

	key_dest = KEY_EDITOR_MATERIAL;

	mPreviewModel = Mod_ForName("models/placeholders/sphere.md2");
	if (!mPreviewModel)
	{
		Con_Warning("Failed to load preview mesh!\n");
		return;
	}

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

void MaterialEditor_Draw(void)
{
	VideoObjectVertex_t	voGrid[4] = { 0 };

	R_SetupGenericView();

	r_framecount++;

	R_SetupScene();

	Video_ResetCapabilities(false);
	glPushMatrix();
	Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);
	glTranslatef(0, 0, 0);

	Video_SetColour(1.0f, 1.0f, 1.0f, 1.0f);
	Video_ObjectVertex(&voGrid[0], -20.0f, 20.0f, 0);
	Video_ObjectTexture(&voGrid[0], VIDEO_TEXTURE_DIFFUSE, 0, 0);
	Video_ObjectVertex(&voGrid[1], 20.0f, 20.0f, 0);
	Video_ObjectTexture(&voGrid[1], VIDEO_TEXTURE_DIFFUSE, 1.0f, 0);
	Video_ObjectVertex(&voGrid[2], 20.0f, -20.0f, 0);
	Video_ObjectTexture(&voGrid[2], VIDEO_TEXTURE_DIFFUSE, 1.0f, 1.0f);
	Video_ObjectVertex(&voGrid[3], -20.0f, -20.0f, 0);
	Video_ObjectTexture(&voGrid[3], VIDEO_TEXTURE_DIFFUSE, 0, 1.0f);
	Video_DrawFill(voGrid, mColour);

	glTranslatef(0, 0, -0.1);
	glPopMatrix();
	Video_ResetCapabilities(true);

	if (mActiveMaterial)
	{
		mPreviewMesh = (MD2_t*)Mod_Extradata(mPreviewModel);

		Video_ResetCapabilities(false);

		glPushMatrix();
		Alias_DrawGenericFrame(mPreviewMesh, mPreviewModel);
		glTranslatef(16, 0, 0);
		glPopMatrix();

		Video_ResetCapabilities(true);
	}
}

void MaterialEditor_Display(Material_t *mDisplayMaterial)
{
	if (!bMaterialEditorInitialized)
	{
		Con_Warning("The editor hasn't been initialized!\n");
		return;
	}

	if (mActiveMaterial)
		Material_Clear(mActiveMaterial);

	mActiveMaterial = mDisplayMaterial;
	if (!mActiveMaterial)
	{
		Con_Warning("Failed to display material!");
		return;
	}

	mPreviewModel->mAssignedMaterials = mActiveMaterial;
}

void MaterialEditor_Shutdown(void)
{}