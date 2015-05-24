#include "MaterialEditorBase.h"

#include "platform_module.h"

EngineExport_t *engine;
EngineImport_t *editor;

pINSTANCE engineInstance;

void EngineInterface_Load()
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
		wxExit();
	}
	// Also ensure that the engine version hasn't changed.
	else if (engine->iVersion != ENGINE_VERSION)
	{
		wxMessageBox("Editor is outdated, please rebuild!", MATERIALEDITOR_TITLE);
		pModule_Unload(engineInstance);
		wxExit();
	}
}

void EngineInterface_Unload()
{
	// Unload the engine module.
	pModule_Unload(engineInstance);
}