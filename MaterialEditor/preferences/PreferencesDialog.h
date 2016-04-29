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

#pragma once

class CPreferencesDialog : public wxDialog
{
public:
	CPreferencesDialog(wxWindow *wParent);
	~CPreferencesDialog();

	virtual bool Show();

	void SyncSettings();
	void ApplySettings();

private:
	// Events
	void HandleCommandEvent(wxCommandEvent &event);

	wxNotebook *Notebook;

	wxCheckBox *AutoReload;
	wxSpinCtrl *AutoReloadDelay;

	wxCheckBox *VideoLegacyMode;
	wxCheckBox *VideoAlphaTrick;

	wxPanel *VideoPanel;
	wxPanel *AudioPanel;
	wxPanel *InputPanel;
	wxPanel *GeneralPanel;

	wxDECLARE_EVENT_TABLE();
};