#include "MaterialEditorBase.h"

#include "MaterialEditorApp.h"

wxIMPLEMENT_APP(CMaterialEditorApp);

bool CMaterialEditorApp::OnInit()
{
	EngineInterface_Load();

	char cTitle[512];
	sprintf(cTitle, MATERIALEDITOR_TITLE" (%s)", engine->GetVersion());

	// Create the main frame.
	mainFrame = new CMaterialEditorFrame(cTitle, wxPoint(50, 50), wxSize(1024, 768));
	
	// Show it!
	mainFrame->Show(true);

	// Initialize the engine.
	if (!engine->Initialize(argc, argv, true))
	{
		wxMessageBox("Failed to initialize engine!", MATERIALEDITOR_TITLE" Error");
		wxExit();
	}

	engine->MaterialEditorInitialize();

	// Start rendering.
	mainFrame->StartEngineLoop();

	// We launched without problems, return true.
	return true;
}

int CMaterialEditorApp::OnExit()
{
	EngineInterface_Unload();
	return 0;
}
