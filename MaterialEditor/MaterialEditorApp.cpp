#include "MaterialEditorBase.h"

#include "MaterialEditorApp.h"
#include "MaterialEditorFrame.h"

wxIMPLEMENT_APP(CMaterialEditorApp);

bool CMaterialEditorApp::OnInit()
{
	if (EngineInterface_Load() == false)
		return false;

	// Create the main frame.
	CMaterialEditorFrame *frame = new CMaterialEditorFrame(MATERIALEDITOR_TITLE, wxPoint(50, 50), wxSize(640, 480));

	// Show it!
	frame->Show(true);

	// We launched without problems, return true.
	return true;
}

int CMaterialEditorApp::OnExit()
{
	EngineInterface_Unload();
	return 0;
}
