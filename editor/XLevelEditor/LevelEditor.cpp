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

#include "LevelEditor.h"

wxIMPLEMENT_APP(XLevelEditor);

ConsoleVariable_t cv_editor_maximize = { "editor_maximize", "1", true };

bool XLevelEditor::OnInit()
{
	plClearLog(XLEVELEDITOR_LOG);

	int width, height;
	wxDisplaySize(&width, &height);

	_main_frame = new XLevelEditorFrame(wxDefaultPosition, wxSize(width / 2, height / 2));
	_main_frame->Show(true);
	
	SetTopWindow(_main_frame);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(XLevelEditorFrame, wxFrame)
wxEND_EVENT_TABLE()

XLevelEditorFrame::XLevelEditorFrame(const wxPoint & pos, const wxSize & size)
	: wxFrame(nullptr, wxID_ANY, XLEVELEDITOR_TITLE, pos, size)
{
	SetIcon(wxIcon("resource/icon-engine.ico", wxBITMAP_TYPE_ICO));

	if(cv_editor_maximize.value)
		Maximize(true);
}
