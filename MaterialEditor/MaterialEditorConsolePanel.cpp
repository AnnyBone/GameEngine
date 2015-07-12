#include "MaterialEditorBase.h"

#include "MaterialEditorConsolePanel.h"

enum
{
	CONSOLE_EVENT_INPUT,
	CONSOLE_EVENT_COMMAND,
	CONSOLE_EVENT_TEXT
};

wxBEGIN_EVENT_TABLE(CMaterialEditorConsolePanel, wxPanel)

EVT_CHAR_HOOK(CMaterialEditorConsolePanel::OnKey)

EVT_BUTTON(CONSOLE_EVENT_COMMAND, CMaterialEditorConsolePanel::OnCommand)

wxEND_EVENT_TABLE()

CMaterialEditorConsolePanel::CMaterialEditorConsolePanel(wxWindow *wParent) 
	: wxPanel(wParent)
{
	wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);

	textConsoleOut = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_DONTWRAP);
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

CMaterialEditorConsolePanel::~CMaterialEditorConsolePanel()
{
}

/*
	Events
*/

void CMaterialEditorConsolePanel::OnCommand(wxCommandEvent &event)
{
	// Send the command to the engine.
	engine->InsertConsoleCommand(wxString(textConsoleIn->GetValue()));

	// Clear the input box.
	textConsoleIn->Clear();
}

void CMaterialEditorConsolePanel::OnKey(wxKeyEvent &event)
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

void CMaterialEditorConsolePanel::PrintMessage(const char *ccMessage)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxGREEN));
	textConsoleOut->AppendText(ccMessage);
}

void CMaterialEditorConsolePanel::PrintWarning(const char *ccMessage)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	textConsoleOut->AppendText(ccMessage);
}

void CMaterialEditorConsolePanel::PrintError(const char *ccMessage)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxRED));
	textConsoleOut->AppendText(ccMessage);
}