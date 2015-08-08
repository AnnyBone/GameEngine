#pragma once

class CWADFrame : public wxFrame
{
public:
	CWADFrame(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CWADFrame();

private:
	wxAuiManager *mManager;
};

