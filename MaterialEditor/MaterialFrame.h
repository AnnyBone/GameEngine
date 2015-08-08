#pragma once

class CMaterialFrame : public wxFrame
{
public:
	CMaterialFrame(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CMaterialFrame();

private:
	wxAuiManager *mManager;
};

