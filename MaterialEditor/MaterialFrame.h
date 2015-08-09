#pragma once

class CMaterialFrame : public wxFrame
{
public:

	CMaterialFrame(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CMaterialFrame();

	void LoadMaterial(wxString sFileName);

	void ReloadCurrentFile();

private:

	Material_t *mCurrent;

	time_t tCurrentModified, tLastModified;

	wxString sCurrentFilePath;

	// Events
	void FileEvent(wxCommandEvent &event);

	wxAuiManager *mManager;

	wxDECLARE_EVENT_TABLE();
};

