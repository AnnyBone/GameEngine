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

#include "EditorConsolePanel.h"

enum
{
	CONSOLE_EVENT_INPUT,
	CONSOLE_EVENT_COMMAND,
	CONSOLE_EVENT_TEXT
};

wxBEGIN_EVENT_TABLE(CEditorConsolePanel, wxPanel)

EVT_CHAR_HOOK(CEditorConsolePanel::OnKey)

EVT_BUTTON(CONSOLE_EVENT_COMMAND, CEditorConsolePanel::OnCommand)

wxEND_EVENT_TABLE()

CEditorConsolePanel::CEditorConsolePanel(wxWindow *wParent) 
	: wxPanel(wParent)
{
	wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);

	textConsoleOut = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_DONTWRAP | wxTE_AUTO_URL);
	textConsoleOut->SetBackgroundColour(wxColour(0, 0, 0));
	textConsoleOut->SetForegroundColour(wxColour(0, 255, 0));

	vSizer->Add(textConsoleOut, 1, wxEXPAND | wxTOP | wxBOTTOM | wxLEFT | wxRIGHT);

	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	hSizer->Add(new wxButton(this, CONSOLE_EVENT_COMMAND, "Submit"), 0, wxRIGHT);

	textConsoleIn = new wxTextCtrl(this, CONSOLE_EVENT_INPUT, "");
	hSizer->Add(textConsoleIn, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);

	hSizer->SetSizeHints(this);

	vSizer->Add(hSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);

	SetSizer(vSizer);
	SetSize(wxSize(wxDefaultSize.x, 256));
}

CEditorConsolePanel::~CEditorConsolePanel()
{
}

/*
	Events
*/

void CEditorConsolePanel::OnCommand(wxCommandEvent &event)
{
	// Send the command to the engine.
	engine->InsertConsoleCommand(wxString(textConsoleIn->GetValue()));

	// Clear the input box.
	textConsoleIn->Clear();
}

void CEditorConsolePanel::OnKey(wxKeyEvent &event)
{
	event.DoAllowNextEvent();

	switch (event.GetId())
	{
	case CONSOLE_EVENT_INPUT:
		if (event.GetKeyCode() == WXK_RETURN)
		{
			// Send the command to the engine.
			engine->InsertConsoleCommand(wxString(textConsoleIn->GetValue()));

			// Clear the input box.
			textConsoleIn->Clear();
		}
		break;
	}
}

/*
	Output
*/

void CEditorConsolePanel::PrintMessage(const char *ccMessage)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxGREEN));
	textConsoleOut->AppendText(ccMessage);
}

void CEditorConsolePanel::PrintWarning(const char *ccMessage)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	textConsoleOut->AppendText(ccMessage);
}

void CEditorConsolePanel::PrintError(const char *ccMessage)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxRED));
	textConsoleOut->AppendText(ccMessage);
}