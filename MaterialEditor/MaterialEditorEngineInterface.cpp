#include "MaterialEditorBase.h"

#include "EditorApp.h"

#include "SharedModule.h"

#include "platform_module.h"

EngineExport_t *engine;
EngineImport_t editor;

pINSTANCE engineInstance;

void EngineInterface_PrintMessage(char *text)
{
	wxGetApp().mainFrame->PrintMessage(text);
}

void EngineInterface_PrintWarning(char *text)
{
	wxGetApp().mainFrame->PrintWarning(text);
}

void EngineInterface_PrintError(char *text)
{
	wxGetApp().mainFrame->PrintError(text);
}

void EngineInterface_Load()
{
	editor.PrintError = EngineInterface_PrintError;
	editor.PrintMessage = EngineInterface_PrintMessage;
	editor.PrintWarning = EngineInterface_PrintWarning;

	// Load the module interface for the engine module.
	engine = (EngineExport_t*)pModule_LoadInterface(
		engineInstance,
		"./"MODULE_ENGINE,
		"Engine_Main",
		&editor);
	// Let us know if it failed to load.
	if (!engine)
	{
		wxMessageBox(pError_Get(), EDITOR_TITLE);
		wxExit();
	}
	// Also ensure that the engine version hasn't changed.
	else if (engine->iVersion != ENGINE_VERSION)
	{
		wxMessageBox("Editor is outdated, please rebuild!", EDITOR_TITLE);
		pModule_Unload(engineInstance);
		wxExit();
	}
}

void EngineInterface_Unload()
{
	// Unload the engine module.
	pModule_Unload(engineInstance);
}