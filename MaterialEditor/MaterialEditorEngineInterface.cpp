#include "MaterialEditorBase.h"

#include "platform_module.h"

#include "shared_module.h"
#include "shared_engine.h"

EngineExport_t *engine;
EngineImport_t *editor;

pINSTANCE engineInstance;

bool EngineInterface_Load()
{
	char path[PLATFORM_MAX_PATH];

	// Update the path to point to where we need it.
	sprintf(path, "./%s/%s", PATH_ENGINE, MODULE_ENGINE);

	// Load the module interface for the engine module.
	engine = (EngineExport_t*)pModule_LoadInterface(
		engineInstance,
		path,
		"Engine_Main",
		&editor);
	// Let us know if it failed to load.
	if (!engine)
	{
		wxMessageBox(pError_Get(), MATERIALEDITOR_TITLE);
		return false;
	}
	// Also ensure that the engine version hasn't changed.
	else if (engine->iVersion != ENGINE_VERSION)
	{
		wxMessageBox("Editor is outdated, please rebuild!", MATERIALEDITOR_TITLE);
		pModule_Unload(engineInstance);
		return false;
	}

	return true;
}

void EngineInterface_Unload()
{
	// Unload the engine module.
	pModule_Unload(engineInstance);
}

void EngineInterface_Initialize()
{
	engine->Initialize(0, 0, true);
}

void EngineInterface_Loop()
{
	engine->Loop();
}

void EngineInterface_SetViewportSize(int width, int height)
{
	engine->SetViewportSize(width, height);
}

char *EngineInterface_GetBasePath()
{
	return engine->GetBasePath();
}

char *EngineInterface_GetMaterialPath()
{
	return engine->GetMaterialPath();
}

/*
	Material Editor
*/

void EngineInterface_MaterialEditorInitialize(void)
{
	engine->MaterialEditorInitialize();
}

bool EngineInterface_LoadEditorMaterial(char *cPath)
{
	Material_t *newMat;

	newMat = engine->LoadMaterial(cPath);
	if (newMat)
	{
		engine->MaterialEditorDisplay(newMat);
		return true;
	}

	return false;
}