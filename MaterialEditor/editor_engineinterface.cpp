/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "EditorBase.h"

#include "editor.h"

#include "SharedModule.h"

#include "platform_module.h"

EngineExport_t *engine;
EngineImport_t editor;

PL_INSTANCE engineInstance;

void EngineInterface_PrintMessage(char *text)
{
	wxGetApp().editor_frame->PrintMessage(text);
}

void EngineInterface_PrintWarning(char *text)
{
	wxGetApp().editor_frame->PrintWarning(text);
}

void EngineInterface_PrintError(char *text)
{
	wxGetApp().editor_frame->PrintError(text);
}

void EngineInterface_Load()
{
	editor.PrintError = EngineInterface_PrintError;
	editor.PrintMessage = EngineInterface_PrintMessage;
	editor.PrintWarning = EngineInterface_PrintWarning;

	// Load the module interface for the engine module.
	engine = (EngineExport_t*)plLoadModuleInterface(
		engineInstance,
		"./" MODULE_ENGINE,
		"Engine_Main",
		&editor);
	// Let us know if it failed to load.
	if (!engine)
	{
		wxMessageBox(plGetError(), EDITOR_TITLE);
		wxExit();
	}
	// Also ensure that the engine version hasn't changed.
	else if (engine->iVersion != ENGINE_VERSION)
	{
		wxMessageBox("Editor is outdated, please rebuild!", EDITOR_TITLE);
		plUnloadModule(engineInstance);
		wxExit();
	}
}

void EngineInterface_Unload()
{
	// Unload the engine module.
	plUnloadModule(engineInstance);
}