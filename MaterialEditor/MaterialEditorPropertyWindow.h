#pragma once
class CMaterialEditorPropertyWindow : public wxFrame
{
public:
	CMaterialEditorPropertyWindow(const wxPoint &pos, const wxSize &size);

private:
	void OnExit(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};

