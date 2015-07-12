#pragma once

class CMaterialEditorConsolePanel :	public wxPanel
{
public:
	CMaterialEditorConsolePanel(wxWindow *wParent);
	~CMaterialEditorConsolePanel();

	void PrintMessage(const char *ccMessage);
	void PrintWarning(const char *ccMessage);
	void PrintError(const char *ccMessage);

	wxTextCtrl *textConsoleOut;
	wxTextCtrl *textConsoleIn;

private:
	void OnCommand(wxCommandEvent &event);
	void OnKey(wxKeyEvent &event);

	wxDECLARE_EVENT_TABLE();
};

