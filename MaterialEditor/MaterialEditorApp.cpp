#include "MaterialEditorBase.h"

#include "MaterialEditorApp.h"

wxIMPLEMENT_APP(CMaterialEditorApp);

bool CMaterialEditorApp::OnInit()
{
	pLog_Clear(MATERIALEDITOR_LOG);
	pLog_Write(MATERIALEDITOR_LOG, "Initializing engine interface...\n");

	EngineInterface_Load();

	char cTitle[512];
	sprintf(cTitle, MATERIALEDITOR_TITLE" (%s)", engine->GetVersion());

	pLog_Write(MATERIALEDITOR_LOG, "Creating main frame...\n");

	// Create the main frame.
	mainFrame = new CMaterialEditorFrame(cTitle, wxPoint(50, 50), wxSize(1024, 768));

	pLog_Write(MATERIALEDITOR_LOG, "Displaying main frame...\n");
	
	// Show it!
	mainFrame->Show(true);

	pLog_Write(MATERIALEDITOR_LOG, "Proceeding with engine initialization...\n");

	// Initialize the engine.
	if (!engine->Initialize(argc, argv, true))
	{
		wxMessageBox("Failed to initialize engine!", MATERIALEDITOR_TITLE" Error");
		wxExit();
	}

	engine->MaterialEditorInitialize();

	pLog_Write(MATERIALEDITOR_LOG, "Starting main loop...\n");

	// Start rendering.
	mainFrame->StartEngineLoop();

	// We launched without problems, return true.
	return true;
}

int CMaterialEditorApp::OnExit()
{
	pLog_Write(MATERIALEDITOR_LOG, "Unloading engine interface...\n");
	EngineInterface_Unload();
	pLog_Write(MATERIALEDITOR_LOG, "Done!\n");
	return 0;
}
