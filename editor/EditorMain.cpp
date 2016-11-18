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

#include "EditorMain.h"

enum EditorEvent {
  EDITOR_EVENT_PROPERTIES
} EditorEvent;

wxBEGIN_EVENT_TABLE(EditorFrame, wxFrame)

		EVT_TIMER(-1, EditorFrame::OnTimer)

wxEND_EVENT_TABLE()

EditorFrame::EditorFrame(const wxString &title, const wxPoint &pos, const wxSize &size) :
	wxFrame(NULL, wxID_ANY, title, pos, size) {

}

wxIMPLEMENT_APP(EditorApp)

bool EditorApp::OnInit() {
  plClearLog(EDITOR_LOG);

  plWriteLog(EDITOR_LOG, "Initializing engine module... ");
  if(xenon::Initialize(0, 0) != PL_RESULT_SUCCESS) {
	plWriteLog(EDITOR_LOG, "Engine failed to initialize, check engine log!\n");
	plMessageBox("Editor", "Failed to initialize engine, please check engine log!");

	wxExit();
  }
  plWriteLog(EDITOR_LOG, "OK\n");

  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxGIFHandler);

  plWriteLog(EDITOR_LOG, "Creating main frame...\n");
}
